/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cstring>

#include "component/ui/virtual_texture.hpp"
#include "foundation/utility/arithmetic.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

const u16 page_size = 256;

texture_type make_page_table( const device_type& device, physical_source& source )
{
	physical_source::size_tuple size = source.size();
	u32 width = round_up< page_size >( size._0 );
	u32 height = round_up< page_size >( size._1 );
	// really need to get maximum texture size from device

	uncompressed_image image( width, height, image::rgb_f16 );
	std::memset( image.get(), 0, image.byte_size() );
	image_pyramid pyramid( image );

	// need to loop and set maximum page mip map level (one page_size width*height)

	return device->texture( pyramid, texture::nearest, false );
}

/*texture_type make_page_cache( const device_type&, physical_source& )
{
}*/

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
virtual_texture::virtual_texture( const device_type& device, physical_source& source_ )
	: source( source_ ), page_table( make_page_table( device, source ) ), page_cache( )
{
}

OOE_NAMESPACE_END( ( ooe ) )
