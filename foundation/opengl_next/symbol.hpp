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
	UNPACK_ALIGNMENT			= 0x0CF5,
	PACK_ALIGNMENT				= 0x0D05,

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

	NEAREST						= 0x2600,
	LINEAR						= 0x2601,
	NEAREST_MIPMAP_NEAREST		= 0x2700,
	LINEAR_MIPMAP_LINEAR		= 0x2703,

	TEXTURE_MAG_FILTER			= 0x2800,
	TEXTURE_MIN_FILTER			= 0x2801,

	ALPHA8						= 0x803C,
	LUMINANCE8					= 0x8040,
	LUMINANCE8_ALPHA8			= 0x8045,
	RGB8						= 0x8051,
	RGBA8						= 0x8058,

	BGR							= 0x80E0,
	BGRA						= 0x80E1,

	DEPTH_COMPONENT24			= 0x81A6,

	TEXTURE_MAX_LEVEL			= 0x813D,

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

	ARRAY_BUFFER				= 0x8892,
	ELEMENT_ARRAY_BUFFER		= 0x8893,
	READ_ONLY					= 0x88B8,
	WRITE_ONLY					= 0x88B9,
	READ_WRITE					= 0x88BA,
	STREAM_DRAW					= 0x88E0,
	STREAM_READ					= 0x88E1,
	STATIC_DRAW					= 0x88E4,
	STATIC_READ					= 0x88E5,
	PIXEL_PACK_BUFFER			= 0x88EB,

	FRAGMENT_SHADER				= 0x8B30,
	VERTEX_SHADER				= 0x8B31,
	COMPILE_STATUS				= 0x8B81,
	LINK_STATUS					= 0x8B82,
	VALIDATE_STATUS				= 0x8B83,
	INFO_LOG_LENGTH				= 0x8B84,

	DEPTH_COMPONENT32F			= 0x8CAC,
	COLOR_ATTACHMENT0			= 0x8CE0,
	RENDERBUFFER				= 0x8D41,

	DEPTH_BUFFER_BIT			= 0x00000100,
	COLOR_BUFFER_BIT			= 0x00004000
};

//--- typedefs -------------------------------------------------------------------------------------
typedef const c8* ( GetString_t )( u32 );
typedef void ( Clear_t )( u32 );
typedef void ( PixelStorei_t )( u32, s32 );

typedef void ( GenTextures_t )( s32, u32* );
typedef void ( DeleteTextures_t )( s32, const u32* );
typedef void ( BindTexture_t )( u32, u32 );
typedef void ( TexImage2D_t )( u32, s32, s32, s32, s32, s32, u32, u32, const void* );
typedef void ( TexSubImage2D_t )( u32, s32, s32, s32, s32, s32, u32, u32, const void* );
typedef void ( CompressedTexImage2D_t )( u32, s32, u32, s32, s32, s32, s32, const void* );
typedef void ( CompressedTexSubImage2D_t )( u32, s32, s32, s32, s32, s32, u32, s32, const void* );
typedef void ( TexParameteri_t )( u32, u32, s32 );
typedef void ( GenerateMipmap_t )( u32 );

typedef u32 ( CreateShader_t )( u32 );
typedef void ( DeleteShader_t )( u32 );
typedef void ( ShaderSource_t )( u32, s32, const c8**, const s32* );
typedef void ( CompileShader_t )( u32 );
typedef void ( GetShaderiv_t )( u32, u32, s32* );
typedef void ( GetShaderInfoLog_t )( u32, s32, s32*, c8* );

typedef u32 ( CreateProgram_t )( void );
typedef void ( DeleteProgram_t )( u32 );
typedef void ( AttachShader_t )( u32, u32 );
typedef void ( LinkProgram_t )( u32 );
typedef void ( UseProgram_t )( u32 );
typedef void ( GetProgramiv_t )( u32, u32, s32* );
typedef void ( GetProgramInfoLog_t )( u32, s32, s32*, c8* );

typedef s32 ( GetUniformLocation_t )( u32, const c8* );
typedef void ( Uniform1fv_t )( s32, s32, const f32* );
typedef void ( Uniform2fv_t )( s32, s32, const f32* );
typedef void ( Uniform3fv_t )( s32, s32, const f32* );
typedef void ( Uniform4fv_t )( s32, s32, const f32* );
typedef void ( Uniform1iv_t )( s32, s32, const s32* );
typedef void ( Uniform2iv_t )( s32, s32, const s32* );
typedef void ( Uniform3iv_t )( s32, s32, const s32* );
typedef void ( Uniform4iv_t )( s32, s32, const s32* );
typedef void ( UniformMatrix2fv_t )( s32, s32, u8, const f32* );
typedef void ( UniformMatrix3fv_t )( s32, s32, u8, const f32* );
typedef void ( UniformMatrix4fv_t )( s32, s32, u8, const f32* );

typedef void ( GenBuffers_t )( s32, u32* );
typedef void ( DeleteBuffers_t )( s32, const u32* );
typedef void ( BindBuffer_t )( u32, u32 );
typedef void ( BufferData_t )( u32, up_t, const void*, u32 );
typedef void* ( MapBuffer_t )( u32, u32 );
typedef u8 ( UnmapBuffer_t )( u32 );

typedef void ( GenRenderbuffers_t )( s32, u32* );
typedef void ( DeleteRenderbuffers_t )( s32, const u32* );
typedef void ( BindRenderbuffer_t )( u32, u32 );
typedef void ( RenderbufferStorage_t )( u32, u32, s32, s32 );

typedef void ( GenFramebuffers_t )( s32, u32* );
typedef void ( DeleteFramebuffers_t )( s32, const u32* );
typedef void ( BindFramebuffer_t )( u32, u32 );
typedef void ( BlitFramebuffer_t )( s32, s32, s32, s32, s32, s32, s32, s32, u32, u32 );
typedef void ( FramebufferTexture2D_t )( u32, u32, u32, u32, s32 );
typedef void ( FramebufferRenderbuffer_t )( u32, u32, u32, u32 );

//--- functions ------------------------------------------------------------------------------------
extern GetString_t* GetString;
extern Clear_t* Clear;
extern PixelStorei_t* PixelStorei;

extern GenTextures_t* GenTextures;
extern DeleteTextures_t* DeleteTextures;
extern BindTexture_t* BindTexture;
extern TexImage2D_t* TexImage2D;
extern TexSubImage2D_t* TexSubImage2D;
extern CompressedTexImage2D_t* CompressedTexImage2D;
extern CompressedTexSubImage2D_t* CompressedTexSubImage2D;
extern TexParameteri_t* TexParameteri;
extern GenerateMipmap_t* GenerateMipmap;

extern CreateShader_t* CreateShader;
extern DeleteShader_t* DeleteShader;
extern ShaderSource_t* ShaderSource;
extern CompileShader_t* CompileShader;
extern GetShaderiv_t* GetShaderiv;
extern GetShaderInfoLog_t* GetShaderInfoLog;

extern CreateProgram_t* CreateProgram;
extern DeleteProgram_t* DeleteProgram;
extern AttachShader_t* AttachShader;
extern LinkProgram_t* LinkProgram;
extern UseProgram_t* UseProgram;
extern GetProgramiv_t* GetProgramiv;
extern GetProgramInfoLog_t* GetProgramInfoLog;

extern GetUniformLocation_t* GetUniformLocation;
extern Uniform1fv_t* Uniform1fv;
extern Uniform2fv_t* Uniform2fv;
extern Uniform3fv_t* Uniform3fv;
extern Uniform4fv_t* Uniform4fv;
extern Uniform1iv_t* Uniform1iv;
extern Uniform2iv_t* Uniform2iv;
extern Uniform3iv_t* Uniform3iv;
extern Uniform4iv_t* Uniform4iv;
extern UniformMatrix2fv_t* UniformMatrix2fv;
extern UniformMatrix3fv_t* UniformMatrix3fv;
extern UniformMatrix4fv_t* UniformMatrix4fv;

extern GenBuffers_t* GenBuffers;
extern DeleteBuffers_t* DeleteBuffers;
extern BindBuffer_t* BindBuffer;
extern BufferData_t* BufferData;
extern MapBuffer_t* MapBuffer;
extern UnmapBuffer_t* UnmapBuffer;

extern GenRenderbuffers_t* GenRenderbuffers;
extern DeleteRenderbuffers_t* DeleteRenderbuffers;
extern BindRenderbuffer_t* BindRenderbuffer;
extern RenderbufferStorage_t* RenderbufferStorage;

extern GenFramebuffers_t* GenFramebuffers;
extern DeleteFramebuffers_t* DeleteFramebuffers;
extern BindFramebuffer_t* BindFramebuffer;
extern BlitFramebuffer_t* BlitFramebuffer;
extern FramebufferTexture2D_t* FramebufferTexture2D;
extern FramebufferRenderbuffer_t* FramebufferRenderbuffer;

OOE_NAMESPACE_END( ( ooe )( opengl ) )

#endif	// OOE_FOUNDATION_OPENGL_NEXT_SYMBOL_HPP
