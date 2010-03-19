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
	Uniform1fv = find< Uniform1fv_t >( "glUniform1fv" );
	Uniform2fv = find< Uniform2fv_t >( "glUniform2fv" );
	Uniform3fv = find< Uniform3fv_t >( "glUniform3fv" );
	Uniform4fv = find< Uniform4fv_t >( "glUniform4fv" );
	Uniform1iv = find< Uniform1iv_t >( "glUniform1iv" );
	Uniform2iv = find< Uniform2iv_t >( "glUniform2iv" );
	Uniform3iv = find< Uniform3iv_t >( "glUniform3iv" );
	Uniform4iv = find< Uniform4iv_t >( "glUniform4iv" );
	UniformMatrix2fv = find< UniformMatrix2fv_t >( "glUniformMatrix2fv" );
	UniformMatrix3fv = find< UniformMatrix3fv_t >( "glUniformMatrix3fv" );
	UniformMatrix4fv = find< UniformMatrix4fv_t >( "glUniformMatrix4fv" );

	GenBuffers = find< GenBuffers_t >( "glGenBuffers" );
	DeleteBuffers = find< DeleteBuffers_t >( "glDeleteBuffers" );
	BindBuffer = find< BindBuffer_t >( "glBindBuffer" );
	BufferData = find< BufferData_t >( "glBufferData" );
	MapBuffer = find< MapBuffer_t >( "glMapBuffer" );
	UnmapBuffer = find< UnmapBuffer_t >( "glUnmapBuffer" );

	GenRenderbuffers = find< GenRenderbuffers_t >( "glGenRenderbuffersEXT" );
	DeleteRenderbuffers = find< DeleteRenderbuffers_t >( "glDeleteRenderbuffersEXT" );
	BindRenderbuffer = find< BindRenderbuffer_t >( "glBindRenderbufferEXT" );
	RenderbufferStorage = find< RenderbufferStorage_t >( "glRenderbufferStorageEXT" );

	GenFramebuffers = find< GenFramebuffers_t >( "glGenFramebuffers" );
	DeleteFramebuffers = find< DeleteFramebuffers_t >( "glDeleteFramebuffers" );
	BindFramebuffer = find< BindFramebuffer_t >( "glBindFramebuffer" );
	BlitFramebuffer = find< BlitFramebuffer_t >( "glBlitFramebuffer" );

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
Uniform1fv_t* Uniform1fv;
Uniform2fv_t* Uniform2fv;
Uniform3fv_t* Uniform3fv;
Uniform4fv_t* Uniform4fv;
Uniform1iv_t* Uniform1iv;
Uniform2iv_t* Uniform2iv;
Uniform3iv_t* Uniform3iv;
Uniform4iv_t* Uniform4iv;
UniformMatrix2fv_t* UniformMatrix2fv;
UniformMatrix3fv_t* UniformMatrix3fv;
UniformMatrix4fv_t* UniformMatrix4fv;

GenBuffers_t* GenBuffers;
DeleteBuffers_t* DeleteBuffers;
BindBuffer_t* BindBuffer;
BufferData_t* BufferData;
MapBuffer_t* MapBuffer;
UnmapBuffer_t* UnmapBuffer;

GenRenderbuffers_t* GenRenderbuffers;
DeleteRenderbuffers_t* DeleteRenderbuffers;
BindRenderbuffer_t* BindRenderbuffer;
RenderbufferStorage_t* RenderbufferStorage;

GenFramebuffers_t* GenFramebuffers;
DeleteFramebuffers_t* DeleteFramebuffers;
BindFramebuffer_t* BindFramebuffer;
BlitFramebuffer_t* BlitFramebuffer;

OOE_NAMESPACE_END( ( ooe )( opengl ) )
