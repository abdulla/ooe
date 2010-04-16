/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/utility/error.hpp"
#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- image_pyramid --------------------------------------------------------------------------------
image_pyramid::image_pyramid( const ooe::image& i )
	: width( i.width ), height( i.height ), format( i.format ), vector( 1, i.ptr() )
{
}

image_pyramid::image_pyramid( u32 width_, u32 height_, ooe::image::type format_ )
	: width( width_ ), height( height_ ), format( format_ ), vector()
{
}

void image_pyramid::push_back( const ooe::image& i )
{
	u32 level_width = width >> vector.size();
	u32 level_height = height >> vector.size();

	if ( i.width != level_width || i.height != level_height )
		throw error::runtime( "image_pyramid: " ) <<
			"Image size " << i.width << 'x' << i.height << " != " <<
			level_width << 'x' << level_height << " required for level " << vector.size();
	else if ( i.format != format )
		throw error::runtime( "image_pyramid: " ) <<
			"Image format " << i.format << " != " << format;

	vector.push_back( i.ptr() );
}

ooe::image image_pyramid::image( u8 level ) const
{
	return ooe::image( width >> level, height >> level, format, vector[ level ] );
}

u8 image_pyramid::size( void ) const
{
	return vector.size();
}

shared_free< void > image_pyramid::operator []( u8 level ) const
{
	return vector[ level ];
}

//--- map ------------------------------------------------------------------------------------------
map::map( void* data_, up_t size_ )
	: data( data_ ), size( size_ )
{
}

OOE_NAMESPACE_END( ( ooe ) )
