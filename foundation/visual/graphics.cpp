/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cstring>

#include "foundation/utility/error.hpp"
#include "foundation/utility/miscellany.hpp"
#include "foundation/visual/graphics.hpp"
#include "foundation/visual/graphics_forward.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- texture_function -----------------------------------------------------------------------------
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

//--- variable_function ----------------------------------------------------------------------------
void variable_function
	( opaque_ptr& pointer, const std::string& name, variable::type mode, const void* data, u8 size )
{
	variable_handle::tuple_type tuple;
	tuple._0 = name;
	tuple._1 = mode;
	std::memcpy( tuple._2, data, size );
	pointer.as< variable_handle >()->vector.push_back( tuple );
}

//--- texture_handle -------------------------------------------------------------------------------
texture_handle::texture_handle( u32 width_, u32 height_, image::type format_ )
	: width( width_ ), height( height_ ), format( format_ ), vector()
{
}

//--- shader_handle --------------------------------------------------------------------------------
shader_handle::shader_handle( shader::type type_, const std::string& text_ )
	: type( type_ ), text( text_ )
{
}

//--- buffer_handle --------------------------------------------------------------------------------
buffer_handle::buffer_handle( buffer::type type_, const void* data_, up_t size_ )
	: type( type_ ), size( size_ ), data( new u8[ size ] )
{
	std::memcpy( data, data_, size );
}

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

bool texture::transformed( void )
{
	return pointer.destructor() ==
		static_cast< opaque_ptr::function_type >( destroy< texture_handle > );
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

//--- batch ----------------------------------------------------------------------------------------
void batch::insert( const texture& texture )
{
	textures.push_back( texture );
}

void batch::insert( const shader& shader )
{
	shaders.push_back( shader );
}

void batch::insert( const buffer& buffer )
{
	buffers.push_back( buffer );
}

OOE_NAMESPACE_END( ( ooe ) )
