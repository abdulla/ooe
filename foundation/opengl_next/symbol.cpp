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

	CreateShader = find< CreateShader_t >( "glCreateShader" );
	DeleteShader = find< DeleteShader_t >( "glDeleteShader" );
	ShaderSource = find< ShaderSource_t >( "glShaderSource" );
	CompileShader = find< CompileShader_t >( "glCompileShader" );
	GetShaderiv = find< GetShaderiv_t >( "glGetShaderiv" );
	GetShaderInfoLog = find< GetShaderInfoLog_t >( "glGetShaderInfoLog" );

	CreateProgram = find< CreateProgram_t >( "glCreateProgram" );
	DeleteProgram = find< DeleteProgram_t >( "glDeleteProgram" );
	AttachShader = find< AttachShader_t >( "glAttachShader" );
	LinkProgram = find< LinkProgram_t >( "glLinkProgram" );
	UseProgram = find< UseProgram_t >( "glUseProgram" );
	GetProgramiv = find< GetProgramiv_t >( "glGetProgramiv" );
	GetProgramInfoLog = find< GetProgramInfoLog_t >( "glGetProgramInfoLog" );

	GetUniformLocation = find< GetUniformLocation_t >( "glGetUniformLocation" );

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

CreateShader_t* CreateShader;
DeleteShader_t* DeleteShader;
ShaderSource_t* ShaderSource;
CompileShader_t* CompileShader;
GetShaderiv_t* GetShaderiv;
GetShaderInfoLog_t* GetShaderInfoLog;

CreateProgram_t* CreateProgram;
DeleteProgram_t* DeleteProgram;
AttachShader_t* AttachShader;
LinkProgram_t* LinkProgram;
UseProgram_t* UseProgram;
GetProgramiv_t* GetProgramiv;
GetProgramInfoLog_t* GetProgramInfoLog;

GetUniformLocation_t* GetUniformLocation;

OOE_NAMESPACE_END( ( ooe )( opengl ) )
