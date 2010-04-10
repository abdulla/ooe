/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cmath>
#include <cstring>

#include "component/ui/virtual_texture.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/utility/arithmetic.hpp"
#include "foundation/utility/binary.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

const image::type table_format = image::rgb_f32;
const c8 shader_name[] = "virtual_texture.fs";

uncompressed_image table_image( u32 size )
{
	uncompressed_image image( size, size, table_format );

	for ( f32* i = image.as< f32 >(), * end = i + size * size * image.channels(); i != end; ++i )
		*i = -1;

	return image;
}

shader_type make_shader( const device_type& device, const std::string& name )
{
	std::string path = executable::path()._0 + "../resource/glsl/" + name;
	return device->shader( name, path, shader::fragment );
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- virtual_texture ------------------------------------------------------------------------------
virtual_texture::virtual_texture( const device_type& device, physical_source& source_ )
	: source( source_ ), table_levels(), table_size(), cache_size(),
	shader( make_shader( device, shader_name ) ), page_table(), page_cache()
{
	//--- page table -----------------------------------------------------------
	u32 page_size = source.page_size();

	if ( !is_bit_round( page_size ) )
		throw error::runtime( "virtual_texture: " ) <<
			"Page size " << page_size << " is not a power of 2";

	table_size = ceiling( source.size(), page_size );
	u32 size_limit = device->limit( device::texture_size );

	if ( table_size > size_limit )
		throw error::runtime( "virtual_texture: " ) <<
			"Page table size " << table_size << " > device texture size " << size_limit;

	image_pyramid table_pyramid( table_image( table_size ) );
	u32 i = 1;

	for ( u32 x = table_size >> 1; x; x >>= 1, ++i )
		table_pyramid.push_back( table_image( x ) );

	table_levels = i + 1;
	page_table = device->texture( table_pyramid, texture::nearest, false );

	//--- page cache -----------------------------------------------------------
	cache_size = round_down( size_limit, page_size );
	image_pyramid cache_pyramid( cache_size, cache_size, source.format() );
	page_cache = device->texture( cache_pyramid, texture::nearest, false );
}

program_type virtual_texture::program( const device_type& device, shader_vector& vector ) const
{
	vector.push_back( shader );
	return device->program( vector );
}

block_type virtual_texture::block( const program_type& program_, const buffer_type& buffer ) const
{
	block_type block_ = program_->block( buffer );
	block_->input( "page_table", page_table );
	block_->input( "page_cache", page_cache );
	return block_;
}

void virtual_texture::load( u32 x, u32 y, u32 width, u32 height, u8 level )
{
	u32 size = source.size();

	if ( level >= table_levels )
		throw error::runtime( "virtual_texture: " ) <<
			"Mipmap level " << level << " > maximum level " << table_levels - 1;
	else if ( x + width > size || y + height > size )
		throw error::runtime( "virtual_texture: " ) <<
			"Load of (" << x << ", " << y << ", " << width << ", " << height <<
			") is outside of source size " << size << " at level " << level;

	u32 page_size = source.page_size();
	u32 page_x1 = floor( x >> level, page_size );
	u32 page_y1 = floor( y >> level, page_size );
	u32 page_x2 = ceiling( ( x + width ) >> level, page_size );
	u32 page_y2 = ceiling( ( y + height ) >> level, page_size );

	uncompressed_image table_image( page_x2 - page_x1, page_y2 - page_y1, table_format );
	f32* update = table_image.as< f32 >();

	// TODO: limit the load by some number of texels
	for ( u32 j = page_y1; j != page_y2; ++j )
	{
		for ( u32 i = page_x1; i != page_x2; ++i, update += table_image.channels() )
		{
			image cache_image = source.read( i, j, level );
			page_cache->write( cache_image, 0, 0, 0 );

			update[ 0 ] = 0;	// should be multiplied by 1 / cache_size
			update[ 1 ] = 0;	// ditto
			update[ 2 ] = std::pow( 2, level ) / table_size;	// should be 2^(mipmap level)
		}
	}

	page_table->write( table_image, page_x1, page_y1, level );
}

OOE_NAMESPACE_END( ( ooe ) )
