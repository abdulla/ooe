/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cstring>

#include "component/ui/virtual_texture.hpp"
#include "foundation/utility/arithmetic.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

const u16 page_size = 256;

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

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- virtual_texture ------------------------------------------------------------------------------
virtual_texture::virtual_texture
	( const device_type& device, physical_source& source_, u32 frame_width, u32 frame_height )
	: source( source_ ), pages_per_side(), position( 0 ), page_table(), page_cache(), readback( 3 )
{
	//--- page table -----------------------------------------------------------
	physical_source::size_tuple size = source.size();
	u32 width = round_up< page_size >( size._0 );
	u32 height = round_up< page_size >( size._1 );
	u32 size_limit = device->limit( device::texture_size );

	if ( width > size_limit || height > size_limit )
		throw error::runtime( "virtual_texture: " ) << "Page table size " << width << 'x' <<
			height << " > device texture size " << size_limit;

	image_pyramid table_pyramid( blank_image( width, height, image::rgb_f16 ) );

	for ( u32 x = width >> 1, y = width >> 1; x || y; x >>= 1, y >>= 1 )
		table_pyramid.push_back( blank_image( x, y, image::rgb_f16 ) );

	page_table = device->texture( table_pyramid, texture::nearest, false );

	//--- page cache -----------------------------------------------------------
	pages_per_side = round_down< page_size >( size_limit );
	u32 pyramid_size = pages_per_side * page_size;
	image_pyramid cache_pyramid( blank_image( pyramid_size, pyramid_size, source.format() ) );
	page_cache = device->texture( cache_pyramid, texture::nearest, false );

	//--- read-back ------------------------------------------------------------
	u32 readback_size = frame_width * frame_height * 4;

	for ( readback_vector::iterator i = readback.begin(), end = readback.end(); i != end; ++i )
		*i = device->buffer( readback_size, buffer::pixel, buffer::stream_read );
}

OOE_NAMESPACE_END( ( ooe ) )
