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
	GetIntegerv = find< GetIntegerv_t >( "glGetIntegerv" );
	Enable = find< Enable_t >( "glEnable" );
	Disable = find< Disable_t >( "glDisable" );
	BlendFunc = find< BlendFunc_t >( "glBlendFunc" );
	PixelStorei = find< PixelStorei_t >( "glPixelStorei" );
	Clear = find< Clear_t >( "glClear" );

	GenTextures = find< GenTextures_t >( "glGenTextures" );
	DeleteTextures = find< DeleteTextures_t >( "glDeleteTextures" );
	BindTexture = find< BindTexture_t >( "glBindTexture" );
	ActiveTexture = find< ActiveTexture_t >( "glActiveTexture" );
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
	ValidateProgram = find< ValidateProgram_t >( "glValidateProgram" );
	UseProgram = find< UseProgram_t >( "glUseProgram" );
	GetProgramiv = find< GetProgramiv_t >( "glGetProgramiv" );
	GetProgramInfoLog = find< GetProgramInfoLog_t >( "glGetProgramInfoLog" );

	GetUniformLocation = find< GetUniformLocation_t >( "glGetUniformLocation" );
	Uniform1iv = find< Uniform1iv_t >( "glUniform1iv" );
	Uniform3iv = find< Uniform3iv_t >( "glUniform3iv" );
	Uniform1fv = find< Uniform1fv_t >( "glUniform1fv" );
	Uniform3fv = find< Uniform3fv_t >( "glUniform3fv" );
	UniformMatrix3fv = find< UniformMatrix3fv_t >( "glUniformMatrix3fv" );
	UniformMatrix4fv = find< UniformMatrix4fv_t >( "glUniformMatrix4fv" );
	GetAttribLocation = find< GetAttribLocation_t >( "glGetAttribLocation" );
	GetFragDataLocation = find< GetFragDataLocation_t >( "glGetFragDataLocationEXT" );

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

	GenFramebuffers = find< GenFramebuffers_t >( "glGenFramebuffersEXT" );
	DeleteFramebuffers = find< DeleteFramebuffers_t >( "glDeleteFramebuffersEXT" );
	BindFramebuffer = find< BindFramebuffer_t >( "glBindFramebufferEXT" );
	BlitFramebuffer = find< BlitFramebuffer_t >( "glBlitFramebufferEXT" );
	FramebufferTexture2D = find< FramebufferTexture2D_t >( "glFramebufferTexture2DEXT" );
	FramebufferRenderbuffer = find< FramebufferRenderbuffer_t >( "glFramebufferRenderbufferEXT" );
	CheckFramebufferStatus = find< CheckFramebufferStatus_t >( "glCheckFramebufferStatusEXT" );
	ReadBuffer = find< ReadBuffer_t >( "glReadBuffer" );
	ReadPixels = find< ReadPixels_t >( "glReadPixels" );

	EnableVertexAttribArray = find< EnableVertexAttribArray_t >( "glEnableVertexAttribArray" );
	DisableVertexAttribArray = find< DisableVertexAttribArray_t >( "glDisableVertexAttribArray" );
	VertexAttribPointer = find< VertexAttribPointer_t >( "glVertexAttribPointer" );
	DrawBuffers = find< DrawBuffers_t >( "glDrawBuffers" );
	DrawElements = find< DrawElements_t >( "glDrawElements" );

	/*const c8* extensions = */GetString( EXTENSIONS );
}

//--- functions ------------------------------------------------------------------------------------
GetString_t* GetString;
GetIntegerv_t* GetIntegerv;
Enable_t* Enable;
Disable_t* Disable;
BlendFunc_t* BlendFunc;
PixelStorei_t* PixelStorei;
Clear_t* Clear;

GenTextures_t* GenTextures;
DeleteTextures_t* DeleteTextures;
BindTexture_t* BindTexture;
ActiveTexture_t* ActiveTexture;
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
ValidateProgram_t* ValidateProgram;
UseProgram_t* UseProgram;
GetProgramiv_t* GetProgramiv;
GetProgramInfoLog_t* GetProgramInfoLog;

GetUniformLocation_t* GetUniformLocation;
Uniform1iv_t* Uniform1iv;
Uniform3iv_t* Uniform3iv;
Uniform1fv_t* Uniform1fv;
Uniform3fv_t* Uniform3fv;
UniformMatrix3fv_t* UniformMatrix3fv;
UniformMatrix4fv_t* UniformMatrix4fv;
GetAttribLocation_t* GetAttribLocation;
GetFragDataLocation_t* GetFragDataLocation;

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
FramebufferTexture2D_t* FramebufferTexture2D;
FramebufferRenderbuffer_t* FramebufferRenderbuffer;
CheckFramebufferStatus_t* CheckFramebufferStatus;
ReadBuffer_t* ReadBuffer;
ReadPixels_t* ReadPixels;

EnableVertexAttribArray_t* EnableVertexAttribArray;
DisableVertexAttribArray_t* DisableVertexAttribArray;
VertexAttribPointer_t* VertexAttribPointer;
DrawBuffers_t* DrawBuffers;
DrawElements_t* DrawElements;

OOE_NAMESPACE_END( ( ooe )( opengl ) )
