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
	texture_generic& texture = *pointer.as< texture_generic >();
	texture_verify( texture, image, level );
	texture_generic::vector_type& vector = texture.vector;

	if ( level == texture::generate_mipmap )
	{
		texture.generate_mipmap = true;
		level = 0;
	}
	else if ( texture.generate_mipmap )
		throw error::runtime( "texture: " ) << "Can not set mipmap level " << level <<
			", automatic mipmap generation has been specified";

	if ( level == vector.size() )
		vector.resize( level + 1 );
	else if ( level > vector.size() )
		throw error::runtime( "texture: " ) <<
			"Mipmap levels below " << level << " must be filled first";

	vector[ level ] = image.ptr();
}

//--- variable_function ----------------------------------------------------------------------------
void variable_function
	( opaque_ptr& pointer, const std::string& name, variable::type mode, const void* data, u8 size )
{
	variable_generic::tuple_type tuple;
	tuple._0 = name;
	tuple._1 = mode;
	std::memcpy( tuple._2, data, size );
	pointer.as< variable_generic >()->vector.push_back( tuple );
}

//--- texture_generic ------------------------------------------------------------------------------
texture_generic::
	texture_generic( u32 width_, u32 height_, image::type format_, texture::type filter_ )
	: width( width_ ), height( height_ ), format( format_ ), filter( filter_ ),
	generate_mipmap( false ), vector()
{
}

//--- shader_generic -------------------------------------------------------------------------------
shader_generic::shader_generic( shader::type type_, const std::string& text_ )
	: type( type_ ), text( text_ )
{
}

//--- buffer_generic -------------------------------------------------------------------------------
buffer_generic::buffer_generic( buffer::type type_, const void* data_, up_t size_ )
	: type( type_ ), size( size_ ), data( new u8[ size ] )
{
	std::memcpy( data, data_, size );
}

//--- texture --------------------------------------------------------------------------------------
texture::texture( u32 width, u32 height, image::type format, type filter )
	: pointer( new texture_generic( width, height, format, filter ), destroy< texture_generic > ),
	function( texture_function )
{
}

void texture::load( const image& image, u8 level )
{
	function( pointer, image, level );
}

bool texture::is_generic( void )
{
	return pointer.destructor() ==
		static_cast< opaque_ptr::function_type >( destroy< texture_generic > );
}

//--- shader ---------------------------------------------------------------------------------------
shader::shader( type mode, const std::string& text )
	: pointer( new shader_generic( mode, text ), destroy< shader_generic > )
{
}

//--- variable -------------------------------------------------------------------------------------
variable::variable( void )
	: pointer( new variable_generic, destroy< variable_generic > ), function( variable_function )
{
}

void variable::insert( const std::string& name, type mode, const void* data, u8 size )
{
	function( pointer, name, mode, data, size );
}

//--- buffer ---------------------------------------------------------------------------------------
buffer::buffer( type mode, const void* data, up_t size )
	: pointer( new buffer_generic( mode, data, size ), destroy< buffer_generic > )
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
