/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl_next/frame.hpp"
#include "foundation/opengl_next/symbol.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- frame ----------------------------------------------------------------------------------------
frame::frame( void )
	: id()
{
	GenFramebuffers( 1, const_cast< u32* >( &id ) );
}

frame::~frame( void )
{
	DeleteFramebuffers( 1, &id );
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )
