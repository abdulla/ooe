/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/io/memory.hpp"
#include "foundation/opengl_next/shader.hpp"
#include "foundation/opengl_next/symbol.hpp"
#include "foundation/utility/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- get_stage ------------------------------------------------------------------------------------
u32 get_stage( shader::type stage )
{
	switch ( stage )
	{
	case shader::vertex:
		return VERTEX_SHADER;

	case shader::fragment:
		return FRAGMENT_SHADER;

	default:
		throw error::runtime( "opengl::shader: " ) << "Unknown shader type: " << stage;
	}
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- shader ---------------------------------------------------------------------------------------
shader::shader( const std::string& name, const descriptor& desc, type stage )
	: id( CreateShader( get_stage( stage ) ) )
{
	memory memory( desc );
	const c8* string = memory.as< c8 >();
	s32 size = memory.size();
	ShaderSource( id, 1, &string, &size );
	CompileShader( id );

	s32 status;
	GetShaderiv( id, COMPILE_STATUS, &status );

	if ( status )
		return;

	GetShaderiv( id, INFO_LOG_LENGTH, &size );
	scoped_array< c8 > array( new c8[ size ] );
	GetShaderInfoLog( id, size, 0, array );
	throw error::runtime( "opengl::shader: " ) <<
		"Unable to compile \"" << name << "\":\n" << array;
}

shader::~shader( void )
{
	DeleteShader( id );
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )
