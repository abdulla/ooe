/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <cstring>

#include "component/ui/virtual_texture.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/utility/arithmetic.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

const up_t readback_limit = 3;
const image::type table_format = image::rgba_f32;
const image::type readback_format = image::rgba_u8;
const c8 shader_name[] = "virtual_texture.fs";

uncompressed_image blank_image( u32 width, u32 height, image::type format )
{
	uncompressed_image image( width, height, format );
	std::memset( image.get(), 0, image.byte_size() );
	return image;
}

// from "Advanced Virtual Texture Topics" by Mittring
u16 f16( f32 value )
{
	u32 int_float;
	std::memcpy( &int_float, &value, sizeof( int_float ) );

	u32 mantissa = int_float & 0x7fffff;
	s32 exponent = ( s32 )( ( int_float >> 23 ) & 0xff ) - ( s32 )0x7f;
	u32 sign = int_float >> 31;

	return ( u16 )( sign << 15 ) | ( u32 )( ( exponent + 0xf ) << 10 ) | ( mantissa >> 13 );
}

shader_type make_shader( const device_type& device, const std::string& name )
{
	std::string path = executable::path()._0 + "../resource/glsl/" + name;
	return device->shader( name, path, shader::fragment );
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- virtual_texture ------------------------------------------------------------------------------
virtual_texture::virtual_texture( const device_type& device, physical_source& source_,
	u32 frame_width_, u32 frame_height_ )
	: source( source_ ), frame_width( frame_width_ ), frame_height( frame_height_ ),
	cache_dimension(), shader( make_shader( device, shader_name ) ), page_table(), page_cache(),
	fragment_target( device->target( frame_width, frame_height, readback_format ) ),
	writeback_target( device->target( frame_width, frame_height, readback_format ) ),
	readback_list()
{
	//--- page table -----------------------------------------------------------
	physical_source::dimension_type dimension = source.dimension();
	u32 page_size = source.page_size();
	u32 width = ceiling( dimension._0, page_size );
	u32 height = ceiling( dimension._1, page_size );
	u32 size_limit = device->limit( device::texture_size );

	if ( width > size_limit || height > size_limit )
		throw error::runtime( "virtual_texture: " ) << "Page table size " << width << 'x' <<
			height << " > device texture size " << size_limit;

	image_pyramid table_pyramid( blank_image( width, height, table_format ) );

	for ( u32 x = width >> 1, y = height >> 1; x && y; x >>= 1, y >>= 1 )
		table_pyramid.push_back( blank_image( x, y, table_format ) );

	page_table = device->texture( table_pyramid, texture::nearest, false );

	//--- page cache -----------------------------------------------------------
	cache_dimension = round_down( size_limit, page_size );
	image_pyramid cache_pyramid( blank_image( cache_dimension, cache_dimension, source.format() ) );
	page_cache = device->texture( cache_pyramid, texture::nearest, false );

	//--- read-back ------------------------------------------------------------
	u32 readback_size = frame_width * frame_height * 4;

	for ( up_t i = 0; i != readback_limit; ++i )
	{
		readback_list.push_back( device->buffer( readback_size, buffer::pixel ) );
		{
			map_type map = readback_list.back()->map( buffer::write );
			std::memset( map->data, 0, map->size );
		}
	}
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

frame_type virtual_texture::frame( const program_type& program_ ) const
{
	frame_type frame_ = program_->frame( frame_width, frame_height );
	frame_->output( frame::colour, fragment_target );
	frame_->output( frame::colour, writeback_target );
	return frame_;
}

void virtual_texture::write( const frame_type& input, frame_type& output )
{
	input->read( readback_list.back(), readback_format, 1 );
	readback_list.splice( readback_list.end(), readback_list, readback_list.begin() );
	{
		map_type map = readback_list.back()->map( buffer::read );

		for ( u8* i = static_cast< u8* >( map->data ), * end = i + map->size; i != end; i += 4 )
		{
			if ( i[ 0 ] || i[ 1 ] )
				std::cout << "rgba: " <<
					i[ 0 ] << ' ' << i[ 1 ] << ' ' << i[ 2 ] << ' ' << i[ 3 ] << '\n';
		}
	}

	output->write( input );
}

OOE_NAMESPACE_END( ( ooe ) )
