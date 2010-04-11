/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cmath>

#include "component/ui/virtual_texture.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/utility/arithmetic.hpp"
#include "foundation/utility/binary.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

const image::type table_format = image::rgb_f32;
const c8 shader_name[] = "virtual_texture.fs";

u32 cache_width( const device_type& device, u32 page_size )
{
	if ( !is_bit_round( page_size ) )
		throw error::runtime( "physical_cache: " ) <<
			"Page size " << page_size << " is not a power of 2";

	return round_down( device->limit( device::texture_size ), page_size );
}

u32 table_width( const device_type& device, const physical_source& source )
{
	u32 table_size = ceiling< u32 >( source.size(), source.page_size() );
	u32 size_limit = device->limit( device::texture_size );

	if ( table_size > size_limit )
		throw error::runtime( "virtual_texture: " ) <<
			"Page table size " << table_size << " > device texture size " << size_limit;

	return table_size;
}

texture_type make_cache( const device_type& device, image::type format, u32 cache_size )
{
	image_pyramid pyramid( cache_size, cache_size, format );
	return device->texture( pyramid, texture::nearest, false );
}

texture_type make_table( const device_type& device, u32 table_size )
{
	image_pyramid pyramid( uncompressed_image( table_size, table_size, table_format ) );

	for ( u32 x = table_size >> 1; x; x >>= 1 )
		pyramid.push_back( uncompressed_image( x, x, table_format ) );

	return device->texture( pyramid, texture::nearest, false );
}

shader_type make_shader( const device_type& device, const std::string& name )
{
	std::string path = executable::path()._0 + "../resource/glsl/" + name;
	return device->shader( name, path, shader::fragment );
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- physical_cache -------------------------------------------------------------------------------
physical_cache::
	physical_cache( const device_type& device, image::type physical_format, u16 physical_page_size )
	: format_( physical_format ), page_size_( physical_page_size ),
	cache_size( cache_width( device, page_size_ ) ),
	cache( make_cache( device, format_, cache_size ) ), free_list(), used_list()
{
	for ( u32 y = 0; y != cache_size; y += page_size_ )
	{
		for ( u32 x = 0; x != cache_size; x += page_size_ )
			free_list.push_back( cache_tuple( x, y, false ) );
	}
}

image::type physical_cache::format( void ) const
{
	return format_;
}

u16 physical_cache::page_size( void ) const
{
	return page_size_;
}

texture_type physical_cache::texture( void ) const
{
	return cache;
}

physical_cache::write_tuple physical_cache::write( const image& image, bool locked )
{
	used_list.splice( used_list.end(), free_list, free_list.begin() );
	cache_tuple& tuple = used_list.back();
	tuple._2 = locked;

	cache->write( image, tuple._0, tuple._1, 0 );
	return write_tuple( divide( tuple._0, cache_size ), divide( tuple._1, cache_size ) );
}

//--- virtual_texture ------------------------------------------------------------------------------
virtual_texture::
	virtual_texture( const device_type& device, physical_cache& cache_, physical_source& source_ )
	: cache( cache_ ), source( source_ ), table_size( table_width( device, source ) ),
	level_limit( log2( table_size ) ), shader_( make_shader( device, shader_name ) ),
	table( make_table( device, table_size ) )
{
	if ( cache.format() != source.format() )
		throw error::runtime( "virtual_texture: " ) << "Cache image format " << cache.format() <<
			" != source image format " << source.format();
	else if ( cache.page_size() != source.page_size() )
		throw error::runtime( "virtual_texture: " ) << "Cache page size " << cache.page_size() <<
			" != source page size " << source.page_size()

	// load base texture and lock
	image cache_image = source.read( 0, 0, level_limit );
	physical_cache::write_tuple tuple = cache.write( cache_image, true );

	uncompressed_image table_image( 1, 1, table_format );
	f32* rgb = table_image.as< f32 >();
	rgb[ 0 ] = tuple._0;
	rgb[ 1 ] = tuple._1;
	rgb[ 2 ] = divide( std::pow( 2, level_limit ), table_size );
	table->write( table_image, 0, 0, level_limit );
}

shader_type virtual_texture::shader( void ) const
{
	return shader_;
}

texture_type virtual_texture::texture( void ) const
{
	return table;
}

void virtual_texture::load( u32 x, u32 y, u32 width, u32 height, u8 level )
{
	u32 size = source.size();

	if ( level > level_limit )
		throw error::runtime( "virtual_texture: " ) <<
			"Mipmap level " << level << " > maximum level " << level_limit;
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
	f32* rgb = table_image.as< f32 >();
	u8 channels = table_image.channels();

	// TODO: limit the load by some number of texels
	for ( u32 j = page_y1; j != page_y2; ++j )
	{
		for ( u32 i = page_x1; i != page_x2; ++i, rgb += channels )
		{
			image cache_image = source.read( i, j, level );
			physical_cache::write_tuple tuple = cache.write( cache_image, false );

			rgb[ 0 ] = tuple._0;
			rgb[ 1 ] = tuple._1;
			rgb[ 2 ] = divide( std::pow( 2, level ), table_size );
		}
	}

	table->write( table_image, page_x1, page_y1, level );
}

OOE_NAMESPACE_END( ( ooe ) )
