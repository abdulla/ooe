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
	
	TEXTURE_2D					= 0x0DE1,

	UNSIGNED_BYTE				= 0x1401,
	UNSIGNED_SHORT				= 0x1403,
	UNSIGNED_INT				= 0x1405,
	FLOAT						= 0x1406,
	HALF_FLOAT					= 0x140B,

	ALPHA						= 0x1906,
	RGB							= 0x1907,
	RGBA						= 0x1908,
	LUMINANCE					= 0x1909,
	LUMINANCE_ALPHA				= 0x190A,

	EXTENSIONS					= 0x1F03,

	UNSIGNED_INT_8_8_8_8		= 0x8035,
	ALPHA8						= 0x803C,
	LUMINANCE8					= 0x8040,
	LUMINANCE8_ALPHA8			= 0x8045,
	RGB8						= 0x8051,
	RGBA8						= 0x8058,

	BGR							= 0x80E0,
	BGRA						= 0x80E1,

	COMPRESSED_RGBA_S3TC_DXT1	= 0x83F1,
	COMPRESSED_RGBA_S3TC_DXT3	= 0x83F2,
	COMPRESSED_RGBA_S3TC_DXT5	= 0x83F3,

	RGBA32F						= 0x8814,
	RGB32F						= 0x8815,
	ALPHA32F					= 0x8816,
	LUMINANCE32F				= 0x8818,
	LUMINANCE_ALPHA32F			= 0x8819,
	RGBA16F						= 0x881A,
	RGB16F						= 0x881B,
	ALPHA16F					= 0x881C,
	LUMINANCE16F				= 0x881E,
	LUMINANCE_ALPHA16F			= 0x881F,

	DEPTH_BUFFER_BIT			= 0x00000100,
	STENCIL_BUFFER_BIT			= 0x00000400,
	COLOR_BUFFER_BIT			= 0x00004000
};

//--- typedefs -------------------------------------------------------------------------------------
typedef const c8* ( GetString_t )( u32 );
typedef void ( Clear_t )( u32 );

typedef void ( GenTextures_t )( s32, u32* );
typedef void ( DeleteTextures_t )( s32, const u32* );
typedef void ( BindTexture_t )( u32, u32 );
typedef void ( TexImage2D_t )( u32, s32, s32, s32, s32, s32, u32, u32, const void* );
typedef void ( CompressedTexImage2D_t )( u32, s32, u32, s32, s32, s32, s32, const void* );

//--- functions ------------------------------------------------------------------------------------
extern GetString_t* GetString;
extern Clear_t* Clear;

extern GenTextures_t* GenTextures;
extern DeleteTextures_t* DeleteTextures;
extern BindTexture_t* BindTexture;
extern TexImage2D_t* TexImage2D;
extern CompressedTexImage2D_t* CompressedTexImage2D;

OOE_NAMESPACE_END( ( ooe )( opengl ) )

#endif	// OOE_FOUNDATION_OPENGL_NEXT_SYMBOL_HPP
