/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cstring>

#include "component/ui/null_source.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- null_source ----------------------------------------------------------------------------------
null_source::null_source( u32 null_size )
	: size_( null_size )
{
}

null_source::~null_source( void )
{
}

u32 null_source::size( void ) const
{
	return size_;
}

image::type null_source::format( void ) const
{
	return image::rgba_u8;
}

u16 null_source::page_size( void ) const
{
	return 256;
}

image null_source::read( const pyramid_index& index )
{
	// this should be encoded with they x, y, and level
	uncompressed_image image( 256, 256, image::rgba_u8 );
	u8* rgba = image.as< u8 >();
	u8 r = index.x * 256 * 255 / ( size_ >> index.level );
	u8 g = index.y * 256 * 255 / ( size_ >> index.level );

	for ( u32 y = 0; y != image.height; ++y )
	{
		for ( u32 x = 0; x != image.width; ++x, rgba += 4 )
		{
			rgba[ 0 ] = 255 - r;
			rgba[ 1 ] = 255 - g;
			rgba[ 2 ] = 255;
			rgba[ 3 ] = 255;
		}
	}

	return image;
}

OOE_NAMESPACE_END( ( ooe ) )
