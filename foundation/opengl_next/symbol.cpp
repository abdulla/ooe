/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/library.hpp"
#include "foundation/opengl_next/symbol.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe )( opengl ) )

template< typename type >
	type* find( const c8* name )
{
	return library::find< type >( name, library::all );
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe )( opengl ) )

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- load_symbols ---------------------------------------------------------------------------------
void load_symbols( void )
{
	GetString = find< GetString_t >( "glGetString" );

	/*const c8* extensions = */GetString( EXTENSIONS );
}

//--- functions ------------------------------------------------------------------------------------
GetString_t* GetString;

OOE_NAMESPACE_END( ( ooe )( opengl ) )
