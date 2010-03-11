/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_NEXT_SYMBOL_HPP
#define OOE_FOUNDATION_OPENGL_NEXT_SYMBOL_HPP

#include "foundation/utility/fundamental.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- load_symbols ---------------------------------------------------------------------------------
void load_symbols( void );

//--- constants ------------------------------------------------------------------------------------
enum
{
	EXTENSIONS		= 0x1F03
};

//--- typedefs -------------------------------------------------------------------------------------
typedef const c8* ( GetString_t )( u32 );

//--- functions ------------------------------------------------------------------------------------
extern GetString_t* GetString;

OOE_NAMESPACE_END( ( ooe )( opengl ) )

#endif	// OOE_FOUNDATION_OPENGL_NEXT_SYMBOL_HPP
