/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl_next/block.hpp"
#include "foundation/opengl_next/program.hpp"
#include "foundation/opengl_next/shader.hpp"
#include "foundation/opengl_next/symbol.hpp"
#include "foundation/utility/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- program --------------------------------------------------------------------------------------
program::program( const shader_vector& vector )
	: id ( CreateProgram() )
{
	for ( shader_vector::const_iterator i = vector.begin(), end = vector.end(); i != end; ++i )
		AttachShader( id, dynamic_cast< opengl::shader& >( **i ).id );

	LinkProgram( id );

	s32 status;
	GetProgramiv( id, LINK_STATUS, &status );

	if ( status )
		return;

	s32 size;
	GetProgramiv( id, INFO_LOG_LENGTH, &size );
	scoped_array< c8 > array( new c8[ size ] );
	GetProgramInfoLog( id, size, 0, array );
	throw error::runtime( "opengl::program: " ) << "Unable to link: " << array;
}

program::~program( void )
{
	DeleteProgram( id );
}

block_type program::block( void ) const
{
	return new opengl::block( id );
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )
