/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <vector>

#include <cstring>

#include "foundation/utility/error.hpp"
#include "foundation/utility/miscellany.hpp"
#include "foundation/video/video_ng.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

//--- texture_handle -------------------------------------------------------------------------------
struct texture_handle
{
	typedef std::vector< shared_free< void > > vector_type;

	u32 width;
	u32 height;
	image::type format;
	vector_type vector;

	texture_handle( u32 width_, u32 height_, image::type format_ )
		: width( width_ ), height( height_ ), format( format_ ), vector()
	{
	}
};

void texture_function( opaque_ptr& pointer, const image& image, u8 level )
{
	texture_handle& handle = *pointer.as< texture_handle >();
	u32 width = handle.width >> level;
	u32 height = handle.height >> level;

	if ( image.width != width || image.height != width )
		throw error::runtime( "texture: " ) <<
			"Image size does not match texture size for mipmap level " << level << ": expected " <<
			width << 'x' << height << ", got " << image.width << 'x' << image.height;
	else if ( image.format != handle.format )
		throw error::runtime( "texture: " ) << "Image format does not match texture format: " <<
			image.format << " != " << handle.format;

	texture_handle::vector_type& vector = handle.vector;

	if ( vector.size() <= level )
		vector.resize( level + 1 );

	vector[ level ] = image.ptr();
}

//--- variable_handle ------------------------------------------------------------------------------
struct variable_handle
{
	typedef u8 data_type[ sizeof( f32 ) * 4 * 4 ];
	typedef tuple< std::string, variable::type, data_type > tuple_type;
	typedef std::vector< tuple_type > vector_type;

	vector_type vector;
};

void variable_function
	( opaque_ptr& pointer, const std::string& name, variable::type mode, const void* data, u8 size )
{
	variable_handle::tuple_type tuple;
	tuple._0 = name;
	tuple._1 = mode;
	std::memcpy( tuple._2, data, size );
	pointer.as< variable_handle >()->vector.push_back( tuple );
}

//--- shader_handle --------------------------------------------------------------------------------
struct shader_handle
{
	shader::type type;
	std::string text;

	shader_handle( shader::type type_, const std::string& text_ )
		: type( type_ ), text( text_ )
	{
	}
};

//--- buffer_handle --------------------------------------------------------------------------------
struct buffer_handle
{
	buffer::type type;
	up_t size;
	scoped_array< u8 > data;

	buffer_handle( buffer::type type_, const void* data_, up_t size_ )
		: type( type_ ), size( size_ ), data( new u8[ size ] )
	{
		std::memcpy( data, data_, size );
	}
};

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- texture --------------------------------------------------------------------------------------
texture::texture( u32 width, u32 height, image::type format )
	: pointer( new texture_handle( width, height, format ), destroy< texture_handle > ),
	function( texture_function )
{
}

void texture::load( const image& image, u8 level )
{
	function( pointer, image, level );
}

//--- shader ---------------------------------------------------------------------------------------
shader::shader( type mode, const std::string& text )
	: pointer( new shader_handle( mode, text ), destroy< shader_handle > )
{
}

//--- variable -------------------------------------------------------------------------------------
variable::variable( void )
	: pointer( new variable_handle, destroy< variable_handle > ), function( variable_function )
{
}

void variable::insert( const std::string& name, type mode, const void* data, u8 size )
{
	function( pointer, name, mode, data, size );
}

//--- buffer ---------------------------------------------------------------------------------------
buffer::buffer( type mode, const void* data, up_t size )
	: pointer( new buffer_handle( mode, data, size ), destroy< buffer_handle > )
{
}

OOE_NAMESPACE_END( ( ooe ) )
