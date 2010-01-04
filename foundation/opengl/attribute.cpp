/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl/attribute.hpp"
#include "foundation/opengl/symbol.hpp"
#include "foundation/utility/error.hpp"

namespace
{
	using namespace ooe;

	u8 size_f( attribute::type type )
	{
		switch ( type )
		{
		case attribute::float_1:
			return 1;

		case attribute::float_2:
			return 2;

		case attribute::float_3:
			return 3;

		case attribute::float_4:
		case attribute::mat_2:
			return 4;

		case attribute::mat_3:
			return 9;

		case attribute::mat_4:
			return 16;

		default:
			throw error::runtime( "opengl: " ) << "Unknown attribute type: " << type;
		}
	}
}

namespace ooe
{
//--- attribute ----------------------------------------------------------------
	opengl::attribute::attribute( u32 program, const c8* name, type flag )
		: id( GetAttribLocation( program, name ) ), size( size_f( flag ) )
	{
		if ( id == -1 )
			throw error::runtime( "opengl: " ) << "Invalid attribute \"" << name << '\"';
	}

	opengl::attribute::~attribute( void )
	{
	}
}
