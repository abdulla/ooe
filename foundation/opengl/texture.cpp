/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl/symbol.hpp"
#include "foundation/opengl/texture.hpp"
#include "foundation/utility/error.hpp"

namespace ooe
{
//--- opengl::texture ----------------------------------------------------------
	opengl::texture::texture( void )
		: id( 0 )
	{
		GenTextures( 1, &id );
	}

	opengl::texture::~texture( void )
	{
		DeleteTextures( 1, &id );
	}

	void opengl::texture::bind( void )
	{
		BindTexture( TEXTURE_2D, id );
	}

	void opengl::texture::unbind( void )
	{
		BindTexture( TEXTURE_2D, 0 );
	}

	void opengl::texture::environment( environment_type type )
	{
		switch ( type )
		{
		case modulate:
			TexEnvi( TEXTURE_ENV, TEXTURE_ENV_MODE, MODULATE );
			return;

		case decal:
			TexEnvi( TEXTURE_ENV, TEXTURE_ENV_MODE, DECAL );
			return;

		case blend:
			TexEnvi( TEXTURE_ENV, TEXTURE_ENV_MODE, BLEND );
			return;

		case replace:
			TexEnvi( TEXTURE_ENV, TEXTURE_ENV_MODE, REPLACE );
			return;

		default:
			throw error::runtime( "opengl: " ) << "Unknown environment type: " << type;
		};
	}

	void opengl::texture::parameter( parameter_type type )
	{
		u32 key_1;
		u32 key_2;
		s32 value_1;
		s32 value_2;

		switch ( type )
		{
		case nearest:
			key_1 = TEXTURE_MAG_FILTER;
			key_2 = TEXTURE_MIN_FILTER;
			value_1 = NEAREST;
			value_2 = NEAREST;
			break;

		case bilinear:
			key_1 = TEXTURE_MAG_FILTER;
			key_2 = TEXTURE_MIN_FILTER;
			value_1 = LINEAR;
			value_2 = LINEAR;
			break;

		case trilinear:
			key_1 = TEXTURE_MAG_FILTER;
			key_2 = TEXTURE_MIN_FILTER;
			value_1 = LINEAR;
			value_2 = LINEAR_MIPMAP_LINEAR;
			break;

		case repeat:
			key_1 = TEXTURE_WRAP_S;
			key_2 = TEXTURE_WRAP_T;
			value_1 = REPEAT;
			value_2 = REPEAT;
			break;

		case clamp:
			key_1 = TEXTURE_WRAP_S;
			key_2 = TEXTURE_WRAP_T;
			value_1 = CLAMP_TO_EDGE;
			value_2 = CLAMP_TO_EDGE;
			break;

		default:
			throw error::runtime( "opengl: " ) << "Unknown texture parameter: " << type;
		}

		TexParameteri( TEXTURE_2D, key_1, value_1 );
		TexParameteri( TEXTURE_2D, key_2, value_2 );
	}

	void opengl::texture::load( const void* data, u32 width, u32 height, u8 format, u8 flags )
	{
		s32 internal;
		s32 external = 0;
		u32 type = 0;

		u32 block = 0;
		bool compressed = false;

		switch ( format )
		{
		case uncompressed_image::bgr_u8:
			internal = RGB8;
			external = BGR;
			type = UNSIGNED_BYTE;
			break;

		case uncompressed_image::bgra_u8:
			internal = RGBA8;
			external = BGRA;
			type = flags & reverse ? UNSIGNED_INT_8_8_8_8_REV : UNSIGNED_INT_8_8_8_8;
			break;

		//--- u8 ---------------------------------------------------------------
		case uncompressed_image::rgb_u8:
			internal = RGB8;
			external = RGB;
			type = UNSIGNED_BYTE;
			break;

		case uncompressed_image::rgba_u8:
			internal = RGBA8;
			external = RGBA;
			type = flags & reverse ? UNSIGNED_INT_8_8_8_8_REV : UNSIGNED_INT_8_8_8_8;
			break;

		case uncompressed_image::a_u8:
			internal = ALPHA8;
			external = ALPHA;
			type = UNSIGNED_BYTE;
			break;

		case uncompressed_image::y_u8:
			internal = LUMINANCE8;
			external = LUMINANCE;
			type = UNSIGNED_BYTE;
			break;

		case uncompressed_image::ya_u8:
			internal = LUMINANCE8_ALPHA8;
			external = LUMINANCE_ALPHA;
			type = UNSIGNED_BYTE;
			break;

		//--- f16 --------------------------------------------------------------
		case uncompressed_image::rgb_f16:
			internal = RGB16F;
			external = RGB;
			type = HALF_FLOAT;
			break;

		case uncompressed_image::rgba_f16:
			internal = RGBA16F;
			external = RGBA;
			type = HALF_FLOAT;
			break;

		case uncompressed_image::a_f16:
			internal = ALPHA16F;
			external = ALPHA;
			type = HALF_FLOAT;
			break;

		case uncompressed_image::y_f16:
			internal = LUMINANCE16F;
			external = LUMINANCE;
			type = HALF_FLOAT;
			break;

		case uncompressed_image::ya_f16:
			internal = LUMINANCE_ALPHA16F;
			external = LUMINANCE_ALPHA;
			type = HALF_FLOAT;
			break;

		//--- f32 --------------------------------------------------------------
		case uncompressed_image::rgb_f32:
			internal = RGB32F;
			external = RGB;
			type = FLOAT;
			break;

		case uncompressed_image::rgba_f32:
			internal = RGBA32F;
			external = RGBA;
			type = FLOAT;
			break;

		case uncompressed_image::a_f32:
			internal = ALPHA32F;
			external = ALPHA;
			type = FLOAT;
			break;

		case uncompressed_image::y_f32:
			internal = LUMINANCE32F;
			external = LUMINANCE;
			type = FLOAT;
			break;

		case uncompressed_image::ya_f32:
			internal = LUMINANCE_ALPHA16F;
			external = LUMINANCE_ALPHA;
			type = FLOAT;
			break;

		//--- dxt --------------------------------------------------------------
		case compressed_image::rgba_dxt1:
			internal = COMPRESSED_RGBA_S3TC_DXT1;
			block = 8;
			compressed = true;
			break;

		case compressed_image::rgba_dxt3:
			internal = COMPRESSED_RGBA_S3TC_DXT3;
			block = 16;
			compressed = true;
			break;

		case compressed_image::rgba_dxt5:
			internal = COMPRESSED_RGBA_S3TC_DXT5;
			block = 16;
			compressed = true;
			break;

		//----------------------------------------------------------------------
		default:
			throw error::runtime( "opengl: " ) << "Unknown texture format: " << format;
		}

		TexParameteri( TEXTURE_2D, GENERATE_MIPMAP, true );
		TexParameteri( TEXTURE_2D, TEXTURE_MAG_FILTER, LINEAR );
		TexParameteri( TEXTURE_2D, TEXTURE_MIN_FILTER, LINEAR_MIPMAP_LINEAR );
		TexParameteri( TEXTURE_2D, TEXTURE_WRAP_S, CLAMP_TO_EDGE );
		TexParameteri( TEXTURE_2D, TEXTURE_WRAP_T, CLAMP_TO_EDGE );

		if ( compressed )
		{
			u32 size = ( ( width + 3 ) / 4 ) * ( ( height + 3 ) / 4 ) * block;

			if ( flags & reload )
				CompressedTexSubImage2D( TEXTURE_2D, 0, 0, 0, width, height, internal, size, data );
			else
				CompressedTexImage2D( TEXTURE_2D, 0, internal, width, height, 0, size, data );
		}
		else
		{
			if ( flags & reload )
				TexSubImage2D( TEXTURE_2D, 0, 0, 0, width, height, external, type, data );
			else
				TexImage2D( TEXTURE_2D, 0, internal, width, height, 0, external, type, data );
		}
	}
}
