/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl_next/frame.hpp"
#include "foundation/opengl_next/symbol.hpp"
#include "foundation/utility/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- frame ----------------------------------------------------------------------------------------
frame::frame( frame::type format )
	: id()
{
	switch ( format )
	{
	case system:
		const_cast< u32& >( id ) = 0;
		break;

	case create:
		GenFramebuffers( 1, const_cast< u32* >( &id ) );
		break;

	default:
		throw error::runtime( "opengl::frame: " ) << "Unknown frame format: " << format;
	}
}

frame::~frame( void )
{
	DeleteFramebuffers( 1, &id );
}

void frame::draw( const frame_type& )
{
}

void frame::draw( const batch& )
{
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )
