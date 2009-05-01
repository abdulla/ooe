/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_SYMBOL_HPP
#define OOE_FOUNDATION_OPENGL_SYMBOL_HPP

#include "foundation/utility/fundamental.hpp"

namespace ooe
{
	namespace opengl
	{
//--- symbol -------------------------------------------------------------------
		void symbol( void );

//--- constants ----------------------------------------------------------------
		enum
		{
			ZERO						= 0,
			ONE							= 1,

			POINTS						= 0x0000,
			LINES						= 0x0001,
			LINE_STRIP					= 0x0003,
			TRIANGLES					= 0x0004,
			TRIANGLE_STRIP				= 0x0005,
			TRIANGLE_FAN				= 0x0006,
			QUADS						= 0x0007,

			NEVER						= 0x0200,
			LESS						= 0x0201,
			EQUAL						= 0x0202,
			LEQUAL						= 0x0203,
			GREATER						= 0x0204,
			NOTEQUAL					= 0x0205,
			GEQUAL						= 0x0206,
			ALWAYS						= 0x0207,

			SRC_COLOR					= 0x0300,
			ONE_MINUS_SRC_COLOR			= 0x0301,
			SRC_ALPHA					= 0x0302,
			ONE_MINUS_SRC_ALPHA			= 0x0303,
			DST_ALPHA					= 0x0304,
			ONE_MINUS_DST_ALPHA			= 0x0305,
			DST_COLOR					= 0x0306,
			ONE_MINUS_DST_COLOR			= 0x0307,
			SRC_ALPHA_SATURATE			= 0x0308,

			CULL_FACE					= 0x0B44,
			DEPTH_TEST					= 0x0B71,
			ALPHA_TEST					= 0x0BC0,
			BLEND						= 0x0BE2,

			PERSPECTIVE_CORRECTION_HINT	= 0x0C50,

			UNPACK_ALIGNMENT			= 0x0CF5,

			MAX_LIGHTS					= 0x0D31,
			MAX_TEXTURE_SIZE			= 0x0D33,

			TEXTURE_2D					= 0x0DE1,

			NICEST						= 0x1102,

			AMBIENT						= 0x1200,
			DIFFUSE						= 0x1201,
			SPECULAR					= 0x1202,
			POSITION					= 0x1203,

			UNSIGNED_BYTE				= 0x1401,
			UNSIGNED_SHORT				= 0x1403,
			UNSIGNED_INT				= 0x1405,
			FLOAT						= 0x1406,
			HALF_FLOAT					= 0x140B,

			MODELVIEW					= 0x1700,
			PROJECTION					= 0x1701,
			TEXTURE						= 0x1702,

			ALPHA						= 0x1906,
			RGB							= 0x1907,
			RGBA						= 0x1908,
			LUMINANCE					= 0x1909,
			LUMINANCE_ALPHA				= 0x190A,

			REPLACE						= 0x1E01,

			VERSION						= 0x1F02,
			EXTENSIONS					= 0x1F03,

			MODULATE					= 0x2100,
			DECAL						= 0x2101,

			TEXTURE_ENV_MODE			= 0x2200,
			TEXTURE_ENV					= 0x2300,

			NEAREST						= 0x2600,
			LINEAR						= 0x2601,
			NEAREST_MIPMAP_NEAREST		= 0x2700,
			LINEAR_MIPMAP_LINEAR		= 0x2703,

			TEXTURE_MAG_FILTER			= 0x2800,
			TEXTURE_MIN_FILTER			= 0x2801,
			TEXTURE_WRAP_S				= 0x2802,
			TEXTURE_WRAP_T				= 0x2803,

			REPEAT						= 0x2901,

			LIGHT0						= 0x4000,

			FUNC_ADD					= 0x8006,
			MIN							= 0x8007,
			MAX							= 0x8008,
			FUNC_SUBTRACT				= 0x800A,
			FUNC_REVERSE_SUBTRACT		= 0x800B,

			UNSIGNED_INT_8_8_8_8		= 0x8035,
			ALPHA8						= 0x803C,
			LUMINANCE8					= 0x8040,
			LUMINANCE8_ALPHA8			= 0x8045,
			RGB8						= 0x8051,
			RGBA8						= 0x8058,

			VERTEX_ARRAY				= 0x8074,
			NORMAL_ARRAY				= 0x8075,
			COLOR_ARRAY					= 0x8076,
			TEXTURE_COORD_ARRAY			= 0x8078,

			BGR							= 0x80E0,
			BGRA						= 0x80E1,

			MAX_ELEMENTS_VERTICES		= 0x80E8,
			MAX_ELEMENTS_INDICES		= 0x80E9,

			CLAMP_TO_EDGE				= 0x812F,

			GENERATE_MIPMAP				= 0x8191,
			GENERATE_MIPMAP_HINT		= 0x8192,

			UNSIGNED_INT_8_8_8_8_REV	= 0x8367,

			COMPRESSED_RGBA_S3TC_DXT1	= 0x83F1,
			COMPRESSED_RGBA_S3TC_DXT3	= 0x83F2,
			COMPRESSED_RGBA_S3TC_DXT5	= 0x83F3,

			TEXTURE0					= 0x84C0,

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

			MAX_TEXTURE_COORDS			= 0x8871,

			ARRAY_BUFFER				= 0x8892,
			ELEMENT_ARRAY_BUFFER		= 0x8893,
			READ_ONLY					= 0x88B8,
			WRITE_ONLY					= 0x88B9,
			READ_WRITE					= 0x88BA,
			STREAM_DRAW					= 0x88E0,
			STREAM_READ					= 0x88E1,
			STREAM_COPY					= 0x88E2,
			STATIC_DRAW					= 0x88E4,
			STATIC_READ					= 0x88E5,
			STATIC_COPY					= 0x88E6,
			DYNAMIC_DRAW				= 0x88E8,
			DYNAMIC_READ				= 0x88E9,
			DYNAMIC_COPY				= 0x88EA,
			PIXEL_PACK_BUFFER			= 0x88EB,
			PIXEL_UNPACK_BUFFER			= 0x88EC,

			SHADING_LANGUAGE_VERSION	= 0x8B8C,

			FRAGMENT_SHADER				= 0x8B30,
			VERTEX_SHADER				= 0x8B31,
			COMPILE_STATUS				= 0x8B81,
			LINK_STATUS					= 0x8B82,
			VALIDATE_STATUS				= 0x8B83,
			INFO_LOG_LENGTH				= 0x8B84,

			DEPTH_BUFFER_BIT			= 0x00000100,
			STENCIL_BUFFER_BIT			= 0x00000400,
			COLOR_BUFFER_BIT			= 0x00004000
		};

//--- typedefs -----------------------------------------------------------------
/*** gl *******************************/
		typedef const c8* ( * GetString_t )( u32 );
		typedef u8 ( * IsEnabled_t )( u32 );
		typedef void ( * Enable_t )( u32 );
		typedef void ( * Disable_t )( u32 );
		typedef void ( * EnableClientState_t )( u32 );
		typedef void ( * DisableClientState_t )( u32 );
		typedef void ( * EnableVertexAttribArray_t )( u32 );
		typedef void ( * DisableVertexAttribArray_t )( u32 );
		typedef void ( * Hint_t )( u32, u32 );
		typedef void ( * GetIntegerv_t )( u32, s32* );
		typedef void ( * Clear_t )( u32 );
		typedef void ( * ClearColor_t )( f32, f32, f32, f32 );
		typedef void ( * ClearDepth_t )( f64 );
		typedef void ( * Viewport_t )( s32, s32, s32, s32 );
		typedef void ( * LineWidth_t )( f32 );
		typedef void ( * PolygonMode_t )( u32, u32 );
		typedef void ( * MatrixMode_t )( u32 );
		typedef void ( * LoadIdentity_t )( void );
		typedef void ( * LoadMatrixf_t )( const f32* );
		typedef void ( * MultMatrixf_t )( const f32* );
		typedef void ( * BlendEquation_t )( u32 );
		typedef void ( * BlendFunc_t )( u32, u32 );
		typedef void ( * AlphaFunc_t )( u32, f32 );
		typedef void ( * DepthFunc_t )( u32 );
		typedef void ( * VertexPointer_t )( s32, u32, s32, const void* );
		typedef void ( * NormalPointer_t )( u32, s32, const void* );
		typedef void ( * ColorPointer_t )( s32, u32, s32, const void* );
		typedef void ( * TexCoordPointer_t )( s32, u32, s32, const void* );
		typedef void ( * VertexAttribPointer_t )( u32, s32, u32, u8, s32, const void* );
		typedef void ( * DrawElements_t )( u32, s32, u32, const void* );
		typedef void ( * DrawArrays_t )( u32, s32, s32 );

/*** language *************************/
		typedef void ( * DeleteShader_t )( u32 );
		typedef void ( * DetachShader_t )( u32, u32 );
		typedef u32 ( * CreateShader_t )( u32 );
		typedef void ( * ShaderSource_t )( u32, s32, const c8**, const s32* );
		typedef void ( * CompileShader_t )( u32 );
		typedef u32 ( * CreateProgram_t )( void );
		typedef void ( * AttachShader_t )( u32, u32 );
		typedef void ( * LinkProgram_t )( u32 );
		typedef void ( * UseProgram_t )( u32 );
		typedef void ( * DeleteProgram_t )( u32 );
		typedef void ( * ValidateProgram_t )( u32 );
		typedef void ( * GetShaderiv_t )( u32, u32, s32* );
		typedef void ( * GetProgramiv_t )( u32, u32, s32* );
		typedef void ( * GetShaderInfoLog_t )( u32, s32, s32*, c8* );
		typedef void ( * GetProgramInfoLog_t )( u32, s32, s32*, c8* );
		typedef s32 ( * GetUniformLocation_t )( u32, const c8* );
		typedef void ( * Uniform1fv_t )( s32, s32, const f32* );
		typedef void ( * Uniform2fv_t )( s32, s32, const f32* );
		typedef void ( * Uniform3fv_t )( s32, s32, const f32* );
		typedef void ( * Uniform4fv_t )( s32, s32, const f32* );
		typedef void ( * Uniform1iv_t )( s32, s32, const s32* );
		typedef void ( * Uniform2iv_t )( s32, s32, const s32* );
		typedef void ( * Uniform3iv_t )( s32, s32, const s32* );
		typedef void ( * Uniform4iv_t )( s32, s32, const s32* );
		typedef void ( * UniformMatrix2fv_t )( s32, s32, u8, const f32* );
		typedef void ( * UniformMatrix3fv_t )( s32, s32, u8, const f32* );
		typedef void ( * UniformMatrix4fv_t )( s32, s32, u8, const f32* );
		typedef s32 ( * GetAttribLocation_t )( u32, const c8* );

/*** buffer ***************************/
		typedef void ( * GenBuffers_t )( s32, u32* );
		typedef void ( * DeleteBuffers_t )( s32, const u32* );
		typedef void ( * BindBuffer_t )( u32, u32 );
		typedef void ( * BufferData_t )( u32, up_t, const void*, u32 );
		typedef void ( * BufferSubData_t )( u32, up_t, up_t, const void* );
		typedef void* ( * MapBuffer_t )( u32, u32 );
		typedef u8 ( * UnmapBuffer_t )( u32 );

/*** texture **************************/
		typedef void ( * GenTextures_t )( s32, u32* );
		typedef void ( * DeleteTextures_t )( s32, const u32* );
		typedef void ( * BindTexture_t )( u32, u32 );
		typedef void ( * TexImage2D_t )( u32, s32, s32, s32, s32, s32, u32, u32, const void* );
		typedef void ( * TexSubImage2D_t )( u32, s32, s32, s32, s32, s32, u32, u32, const void* );
		typedef void ( * CompressedTexImage2D_t )( u32, s32, u32, s32, s32, s32, s32, const void* );
		typedef void ( * CompressedTexSubImage2D_t )
			( u32, s32, s32, s32, s32, s32, u32, s32, const void* );
		typedef void ( * TexParameteri_t )( u32, u32, s32 );
		typedef void ( * TexEnvi_t )( u32, u32, s32 );
		typedef void ( * PixelStorei_t )( u32, s32 );
		typedef void ( * ActiveTexture_t )( u32 );
		typedef void ( * ClientActiveTexture_t )( u32 );

/*** light ****************************/
		typedef void ( * Lightfv_t )( u32, u32, const f32* );

//--- function pointers --------------------------------------------------------
/*** gl *******************************/
		extern GetString_t GetString;

		extern IsEnabled_t IsEnabled;
		extern Enable_t Enable;
		extern Disable_t Disable;
		extern EnableClientState_t EnableClientState;
		extern DisableClientState_t DisableClientState;
		extern EnableVertexAttribArray_t EnableVertexAttribArray;
		extern DisableVertexAttribArray_t DisableVertexAttribArray;

		extern Hint_t Hint;
		extern GetIntegerv_t GetIntegerv;

		extern Clear_t Clear;
		extern ClearColor_t ClearColor;
		extern ClearDepth_t ClearDepth;

		extern Viewport_t Viewport;
		extern LineWidth_t LineWidth;
		extern PolygonMode_t PolygonMode;

		extern MatrixMode_t MatrixMode;
		extern LoadIdentity_t LoadIdentity;
		extern LoadMatrixf_t LoadMatrixf;
		extern MultMatrixf_t MultMatrixf;

		extern BlendEquation_t BlendEquation;
		extern BlendFunc_t BlendFunc;
		extern AlphaFunc_t AlphaFunc;
		extern DepthFunc_t DepthFunc;

		extern VertexPointer_t VertexPointer;
		extern NormalPointer_t NormalPointer;
		extern ColorPointer_t ColorPointer;
		extern TexCoordPointer_t TexCoordPointer;
		extern VertexAttribPointer_t VertexAttribPointer;

		extern DrawElements_t DrawElements;
		extern DrawArrays_t DrawArrays;

/*** language *************************/
		extern DeleteShader_t DeleteShader;
		extern DetachShader_t DetachShader;
		extern CreateShader_t CreateShader;
		extern ShaderSource_t ShaderSource;
		extern CompileShader_t CompileShader;

		extern CreateProgram_t CreateProgram;
		extern AttachShader_t AttachShader;
		extern LinkProgram_t LinkProgram;
		extern UseProgram_t UseProgram;
		extern DeleteProgram_t DeleteProgram;
		extern ValidateProgram_t ValidateProgram;

		extern GetShaderiv_t GetShaderiv;
		extern GetProgramiv_t GetProgramiv;
		extern GetShaderInfoLog_t GetShaderInfoLog;
		extern GetProgramInfoLog_t GetProgramInfoLog;

		extern GetUniformLocation_t GetUniformLocation;
		extern Uniform1fv_t Uniform1fv;
		extern Uniform2fv_t Uniform2fv;
		extern Uniform3fv_t Uniform3fv;
		extern Uniform4fv_t Uniform4fv;
		extern Uniform1iv_t Uniform1iv;
		extern Uniform2iv_t Uniform2iv;
		extern Uniform3iv_t Uniform3iv;
		extern Uniform4iv_t Uniform4iv;
		extern UniformMatrix2fv_t UniformMatrix2fv;
		extern UniformMatrix3fv_t UniformMatrix3fv;
		extern UniformMatrix4fv_t UniformMatrix4fv;

		extern GetAttribLocation_t GetAttribLocation;

/*** buffer ***************************/
		extern GenBuffers_t GenBuffers;
		extern DeleteBuffers_t DeleteBuffers;
		extern BindBuffer_t BindBuffer;

		extern BufferData_t BufferData;
		extern BufferSubData_t BufferSubData;
		extern MapBuffer_t MapBuffer;
		extern UnmapBuffer_t UnmapBuffer;

/*** texture **************************/
		extern GenTextures_t GenTextures;
		extern DeleteTextures_t DeleteTextures;
		extern BindTexture_t BindTexture;
		extern TexImage2D_t TexImage2D;
		extern TexSubImage2D_t TexSubImage2D;
		extern CompressedTexImage2D_t CompressedTexImage2D;
		extern CompressedTexSubImage2D_t CompressedTexSubImage2D;
		extern TexParameteri_t TexParameteri;
		extern TexEnvi_t TexEnvi;
		extern PixelStorei_t PixelStorei;
		extern ActiveTexture_t ActiveTexture;
		extern ClientActiveTexture_t ClientActiveTexture;

/*** light ****************************/
		extern Lightfv_t Lightfv;
	}
}

#endif	// OOE_FOUNDATION_OPENGL_SYMBOL_HPP
