/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl/light.hpp"
#include "foundation/opengl/symbol.hpp"
#include "foundation/utility/error.hpp"

namespace ooe
{
//--- opengl::light ------------------------------------------------------------
	opengl::light::light( u32 id_, bool& used_ )
		: id( id_ ), used( used_ )
	{
		used = true;
	}

	opengl::light::~light( void )
	{
		used = false;
	}

	void opengl::light::parameter( const f32* values, parameter_type type )
	{
		u32 parameter_;

		switch ( type )
		{
		case ambient:
			parameter_ = AMBIENT;
			break;

		case diffuse:
			parameter_ = DIFFUSE;
			break;

		case specular:
			parameter_ = SPECULAR;
			break;

		case position:
			parameter_ = POSITION;
			break;

		default:
			throw error::runtime( "opengl: " ) << "Unknown parameter type: " << type;
		}

		Lightfv( LIGHT0 + id, parameter_, values );
	}
}
