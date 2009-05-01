/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/io/memory.hpp"
#include "foundation/opengl/shader.hpp"
#include "foundation/opengl/symbol.hpp"
#include "foundation/utility/error.hpp"

namespace ooe
{
//--- opengl::shader -----------------------------------------------------------
	opengl::shader::shader( type value )
		: id( CreateShader( value == vertex ? VERTEX_SHADER : FRAGMENT_SHADER ) )
	{
	}

	opengl::shader::~shader( void )
	{
		DeleteShader( id );
	}

	void opengl::shader::compile( const c8* string, u32 size_ )
	{
		s32 size = size_;
		ShaderSource( id, 1, &string, &size );
		CompileShader( id );

		s32 status;
		GetShaderiv( id, COMPILE_STATUS, &status );

		if ( status )
			return;

		s32 length;
		GetShaderiv( id, INFO_LOG_LENGTH, &length );
		scoped_array< c8 > array( new c8[ length ] );
		GetShaderInfoLog( id, length, 0, array );
		throw error::runtime( "opengl: " ) << array;
	}
}
