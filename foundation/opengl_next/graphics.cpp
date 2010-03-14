/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl_next/symbol.hpp"
#include "foundation/opengl_next/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- graphics -------------------------------------------------------------------------------------
graphics::graphics( const ooe::view_data& view_ )
try
	: view( view_ ), context( context_construct( view ) )
{
	context_current( view, context );
	context_sync( view, context, true );
	load_symbols();

	PixelStorei( PACK_ALIGNMENT, 1 );
	PixelStorei( UNPACK_ALIGNMENT, 1 );
}
catch ( ... )
{
	this->~graphics();
}

graphics::~graphics( void )
{
	context_current( view, 0 );
	context_destruct( view, context );
}

void graphics::draw( frame&, batch& )
{
}

void graphics::swap( void )
{
	context_swap( view, context );
	Clear( DEPTH_BUFFER_BIT | STENCIL_BUFFER_BIT | COLOR_BUFFER_BIT );
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )
