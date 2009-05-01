/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl/attribute.hpp"
#include "foundation/opengl/program.hpp"
#include "foundation/opengl/shader.hpp"
#include "foundation/opengl/symbol.hpp"
#include "foundation/opengl/uniform.hpp"
#include "foundation/utility/error.hpp"

namespace ooe
{
//--- opengl::program ----------------------------------------------------------
	opengl::program::program( void )
		: id( CreateProgram() )
	{
	}

	opengl::program::~program( void )
	{
		DeleteProgram( id );
	}

	void opengl::program::bind( void )
	{
		UseProgram( id );
	}

	void opengl::program::unbind( void )
	{
		UseProgram( 0 );
	}

	void opengl::program::attach( const ooe::shader* shader )
	{
		AttachShader( id, static_cast< const opengl::shader* >( shader )->id );
	}

	void opengl::program::detach( const ooe::shader* shader )
	{
		DetachShader( id, static_cast< const opengl::shader* >( shader )->id );
	}

	void opengl::program::link( void )
	{
		LinkProgram( id );

		s32 status;
		GetProgramiv( id, LINK_STATUS, &status );

		if ( status )
			return;

		s32 size;
		GetProgramiv( id, INFO_LOG_LENGTH, &size );
		scoped_array< c8 > array( new c8[ size ] );
		GetProgramInfoLog( id, size, 0, array );
		throw error::runtime( "opengl: " ) << array;
	}

	ooe::uniform* opengl::program::uniform( const c8* name, uniform::type type ) const
	{
		return new opengl::uniform( id, name, type );
	}

	ooe::attribute* opengl::program::attribute( const c8* name, attribute::type type ) const
	{
		return new opengl::attribute( id, name, type );
	}
}
