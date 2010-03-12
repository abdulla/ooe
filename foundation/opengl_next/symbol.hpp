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
	EXTENSIONS			= 0x1F03,

	DEPTH_BUFFER_BIT	= 0x00000100,
	STENCIL_BUFFER_BIT	= 0x00000400,
	COLOR_BUFFER_BIT	= 0x00004000
};

//--- typedefs -------------------------------------------------------------------------------------
typedef const c8* ( GetString_t )( u32 );
typedef void ( Clear_t )( u32 );

typedef void ( GenTextures_t )( s32, u32* );
typedef void ( DeleteTextures_t )( s32, const u32* );

//--- functions ------------------------------------------------------------------------------------
extern GetString_t* GetString;
extern Clear_t* Clear;

extern GenTextures_t* GenTextures;
extern DeleteTextures_t* DeleteTextures;

OOE_NAMESPACE_END( ( ooe )( opengl ) )

#endif	// OOE_FOUNDATION_OPENGL_NEXT_SYMBOL_HPP
