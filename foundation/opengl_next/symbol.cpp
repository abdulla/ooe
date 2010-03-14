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
	Clear = find< Clear_t >( "glClear" );
	PixelStorei = find< PixelStorei_t >( "glPixelStorei" );

	GenTextures = find< GenTextures_t >( "glGenTextures" );
	DeleteTextures = find< DeleteTextures_t >( "glDeleteTextures" );
	BindTexture = find< BindTexture_t >( "glBindTexture" );
	TexImage2D = find< TexImage2D_t >( "glTexImage2D" );
	TexSubImage2D = find< TexSubImage2D_t >( "glTexSubImage2D" );
	CompressedTexImage2D = find< CompressedTexImage2D_t >( "glCompressedTexImage2D" );
	CompressedTexSubImage2D = find< CompressedTexSubImage2D_t >( "glCompressedTexSubImage2D" );
	TexParameteri = find< TexParameteri_t >( "glTexParameteri" );
	GenerateMipmap = find< GenerateMipmap_t >( "glGenerateMipmapEXT" );

	/*const c8* extensions = */GetString( EXTENSIONS );
}

//--- functions ------------------------------------------------------------------------------------
GetString_t* GetString;
Clear_t* Clear;
PixelStorei_t* PixelStorei;

GenTextures_t* GenTextures;
DeleteTextures_t* DeleteTextures;
BindTexture_t* BindTexture;
TexImage2D_t* TexImage2D;
TexSubImage2D_t* TexSubImage2D;
CompressedTexImage2D_t* CompressedTexImage2D;
CompressedTexSubImage2D_t* CompressedTexSubImage2D;
TexParameteri_t* TexParameteri;
GenerateMipmap_t* GenerateMipmap;

OOE_NAMESPACE_END( ( ooe )( opengl ) )
