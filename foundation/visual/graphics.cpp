/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/utility/error.hpp"
#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- map ------------------------------------------------------------------------------------------
map::map( void* data_, up_t size_ )
	: data( data_ ), size( size_ )
{
}

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

image::data_type image_pyramid::operator []( u8 level ) const
{
	return vector[ level ];
}

//--- shader_include -------------------------------------------------------------------------------
shader_include::shader_include( const device_type& device_, const ooe::vfs& vfs_ )
	: device( device_ ), vfs( vfs_ ), vector()
{
}

void shader_include::push_back( const std::string& path )
{
	vector.push_back( vfs[ path ] );
}

shader_type shader_include::compile( const std::string& path, shader::type type ) const
{
	memory_vector copy = vector;
	copy.push_back( vfs[ path ] );

	try
	{
		return device->shader( copy, type );
	}
	catch ( error::runtime& error )
	{
		throw error::runtime( "shader_include: " ) <<
			"Unable to compile \"" << path << "\": " << error.what();
	}
}

OOE_NAMESPACE_END( ( ooe ) )
