/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/utility/error.hpp"
#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- image_pyramid --------------------------------------------------------------------------------
image_pyramid::image_pyramid( const image& image )
	: width( image.width ), height( image.height ), format( image.format ), vector( 1, image.ptr() )
{
}

image_pyramid::image_pyramid( u32 width_, u32 height_, image::type format_ )
	: width( width_ ), height( height_ ), format( format_ ), vector()
{
}

void image_pyramid::push_back( const image& image )
{
	u32 level_width = width >> vector.size();
	u32 level_height = height >> vector.size();

	if ( image.width != level_width || image.height != level_height )
		throw error::runtime( "image_pyramid: " ) <<
			"Image size " << image.width << 'x' << image.height << " != " <<
			level_width << 'x' << level_height << " required for level " << vector.size();
	else if ( image.format != format )
		throw error::runtime( "image_pyramid: " ) <<
			"Image format " << image.format << " != " << format;

	vector.push_back( image.ptr() );
}

const image_pyramid::vector_type& image_pyramid::get( void ) const
{
	return vector;
}

//--- map ------------------------------------------------------------------------------------------
map::map( void* data_, up_t size_ )
	: data( data_ ), size( size_ )
{
}

OOE_NAMESPACE_END( ( ooe ) )
