/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl_next/symbol.hpp"
#include "foundation/opengl_next/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- graphics -------------------------------------------------------------------------------------
graphics::graphics( const ooe::view_data& view_ )
try
	: view( view_ ), context( context_construct( view ) )
{
	setup_context( view, context );
	load_symbols();
}
catch ( ... )
{
	this->~graphics();
}

void graphics::swap( void )
{
	context_swap( view, context );
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )
