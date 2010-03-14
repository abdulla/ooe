/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl_next/symbol.hpp"
#include "foundation/opengl_next/texture.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/utility/miscellany.hpp"
#include "foundation/visual/graphics_forward.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe )( opengl ) )

typedef tuple< u32, u32 > compressed_tuple;
typedef tuple< u32, u32, u32 > uncompressed_tuple;

//--- open_texture ---------------------------------------------------------------------------------
struct open_texture
	: public texture
{
	using texture::pointer;
	using texture::function;
};

//--- format_compressed ----------------------------------------------------------------------------
compressed_tuple format_compressed( const texture_generic& texture )
{
	u32 size = ( ( texture.width + 3 ) / 4 ) * ( ( texture.height + 3 ) / 4 );

	switch ( texture.format )
	{
	//--- dxt ------------------------------------------------------------------
	case compressed_image::rgba_dxt1:
		return compressed_tuple( COMPRESSED_RGBA_S3TC_DXT1, size * 8 );

	case compressed_image::rgba_dxt3:
		return compressed_tuple( COMPRESSED_RGBA_S3TC_DXT3, size * 16 );

	case compressed_image::rgba_dxt5:
		return compressed_tuple( COMPRESSED_RGBA_S3TC_DXT5, size * 16 );

	//--------------------------------------------------------------------------
	default:
		throw error::runtime( "opengl::texture: " ) <<
			"Unknown compressed image format: " << texture.format;
	}
}

//--- format_uncompressed --------------------------------------------------------------------------
uncompressed_tuple format_uncompressed( image::type format )
{
	switch ( format )
	{
	case uncompressed_image::bgr_u8:
		return uncompressed_tuple( RGB8, BGR, UNSIGNED_BYTE );

	case uncompressed_image::bgra_u8:
		return uncompressed_tuple( RGBA8, BGRA, UNSIGNED_INT_8_8_8_8 );

	//--- u8 -------------------------------------------------------------------
	case uncompressed_image::rgb_u8:
		return uncompressed_tuple( RGB8, RGB, UNSIGNED_BYTE );

	case uncompressed_image::rgba_u8:
		return uncompressed_tuple( RGBA8, RGBA, UNSIGNED_INT_8_8_8_8 );

	case uncompressed_image::a_u8:
		return uncompressed_tuple( ALPHA8, ALPHA, UNSIGNED_BYTE );

	case uncompressed_image::y_u8:
		return uncompressed_tuple( LUMINANCE8, LUMINANCE, UNSIGNED_BYTE );

	case uncompressed_image::ya_u8:
		return uncompressed_tuple( LUMINANCE8_ALPHA8, LUMINANCE_ALPHA, UNSIGNED_BYTE );

	//--- f16 ------------------------------------------------------------------
	case uncompressed_image::rgb_f16:
		return uncompressed_tuple( RGB16F, RGB, HALF_FLOAT );

	case uncompressed_image::rgba_f16:
		return uncompressed_tuple( RGBA16F, RGBA, HALF_FLOAT );

	case uncompressed_image::a_f16:
		return uncompressed_tuple( ALPHA16F, ALPHA, HALF_FLOAT );

	case uncompressed_image::y_f16:
		return uncompressed_tuple( LUMINANCE16F, LUMINANCE, HALF_FLOAT );

	case uncompressed_image::ya_f16:
		return uncompressed_tuple( LUMINANCE_ALPHA16F, LUMINANCE_ALPHA, HALF_FLOAT );

	//--- f32 ------------------------------------------------------------------
	case uncompressed_image::rgb_f32:
		return uncompressed_tuple( RGB32F, RGB, FLOAT );

	case uncompressed_image::rgba_f32:
		return uncompressed_tuple( RGBA32F, RGBA, FLOAT );

	case uncompressed_image::a_f32:
		return uncompressed_tuple( ALPHA32F, ALPHA, FLOAT );

	case uncompressed_image::y_f32:
		return uncompressed_tuple( LUMINANCE32F, LUMINANCE, FLOAT );

	case uncompressed_image::ya_f32:
		return uncompressed_tuple( LUMINANCE_ALPHA16F, LUMINANCE_ALPHA, FLOAT );

	//--------------------------------------------------------------------------
	default:
		throw error::runtime( "opengl::texture: " ) <<
			"Unknown uncompressed image format: " << format;
	}
}

//--- set_filter -----------------------------------------------------------------------------------
void set_filter( texture::type filter )
{
	u32 min_filter;
	u32 mag_filter;

	switch ( filter )
	{
	case texture::nearest:
		min_filter = NEAREST_MIPMAP_NEAREST;
		mag_filter = NEAREST;
		break;

	case texture::linear:
		min_filter = LINEAR_MIPMAP_LINEAR;
		mag_filter = LINEAR;
		break;

	default:
		throw error::runtime( "opengl::texture: " ) << "Unknown filter type: " << filter;
	}

	TexParameteri( TEXTURE_2D, TEXTURE_MIN_FILTER, min_filter );
	TexParameteri( TEXTURE_2D, TEXTURE_MAG_FILTER, mag_filter );
}

//--- texture_api ----------------------------------------------------------------------------------
struct texture_api
{
	const u32 id;

	const u32 width;
	const u32 height;
	const image::type format;
	const u32 levels;

	texture_api( const texture_generic& texture )
		: id(), width( texture.width ), height( texture.height ), format( texture.format ),
		levels( texture.vector.size() )
	{
		GenTextures( 1, const_cast< u32* >( &id ) );

		BindTexture( TEXTURE_2D, id );
		set_filter( texture.filter );
	}

	~texture_api( void )
	{
		DeleteTextures( 1, &id );
	}
};

//--- texture_compressed ---------------------------------------------------------------------------
struct texture_compressed
	: public texture_api
{
	const u32 internal;
	const u32 size;

	texture_compressed( const texture_generic& texture )
		: texture_api( texture ), internal(), size()
	{
		compressed_tuple tuple = format_compressed( texture );
		const_cast< u32& >( internal ) = tuple._0;
		const_cast< u32& >( size ) = tuple._1;
		const texture_generic::vector_type& vector = texture.vector;

		for ( u32 i = 0, end = vector.size(); i != end; ++i )
			CompressedTexImage2D( TEXTURE_2D, i, internal, width, height, 0, size, vector[ i ] );
	}
};

//--- texture_uncompressed -------------------------------------------------------------------------
struct texture_uncompressed
	: public texture_api
{
	const u32 external;
	const u32 type;

	texture_uncompressed( const texture_generic& texture )
		: texture_api( texture ), external(), type()
	{
		uncompressed_tuple tuple = format_uncompressed( format );
		const_cast< u32& >( external ) = tuple._1;
		const_cast< u32& >( type ) = tuple._2;
		const texture_generic::vector_type& vector = texture.vector;

		for ( u32 i = 0, end = vector.size(); i != end; ++i )
			TexImage2D( TEXTURE_2D, i, tuple._0, width, height, 0, external, type, vector[ i ] );
	}
};

//--- mipmap_verify --------------------------------------------------------------------------------
void mipmap_verify( texture_api& texture, u8 level )
{
	if ( level < texture.levels )
		throw error::runtime( "opengl::texture: " ) << "Mipmap level " << level <<
			" is outside the maximum number of levels: " << texture.levels;
}

//--- load_compressed ------------------------------------------------------------------------------
void load_compressed( opaque_ptr& pointer, const image& image, u8 level )
{
	texture_compressed& texture = *pointer.as< texture_compressed >();
	texture_verify( texture, image, level );
	mipmap_verify( texture, level );
	BindTexture( TEXTURE_2D, texture.id );
	CompressedTexSubImage2D( TEXTURE_2D, level, 0, 0, texture.width, texture.height,
		texture.internal, texture.size, image.ptr() );
}

//--- load_uncompressed ----------------------------------------------------------------------------
void load_uncompressed( opaque_ptr& pointer, const image& image, u8 level )
{
	texture_uncompressed& texture = *pointer.as< texture_uncompressed >();
	texture_verify( texture, image, level );
	mipmap_verify( texture, level );
	BindTexture( TEXTURE_2D, texture.id );
	TexSubImage2D( TEXTURE_2D, level, 0, 0, texture.width, texture.height, texture.external,
		texture.type, image.ptr() );
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe )( opengl ) )

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- transform_texture ----------------------------------------------------------------------------
void transform_texture( texture& closed )
{
	if ( !closed.is_generic() )
		return;

	open_texture& open = static_cast< open_texture& >( closed );
	texture_generic& texture = *open.pointer.as< texture_generic >();

	if ( texture.vector.empty() )
		throw error::runtime( "opengl::texture: " ) << "Texture of size " << texture.width << 'x' <<
			texture.height << " and format " << texture.format << " contains no image data";

	if ( is_compressed( texture.format ) )
	{
		opaque_ptr pointer( new texture_compressed( texture ), destroy< texture_compressed > );
		open.pointer.swap( pointer );
		open.function = load_compressed;
	}
	else
	{
		opaque_ptr pointer( new texture_uncompressed( texture ), destroy< texture_uncompressed > );
		open.pointer.swap( pointer );
		open.function = load_uncompressed;
	}

	if ( texture.generate_mipmap )
		GenerateMipmap( TEXTURE_2D );
	else
		TexParameteri( TEXTURE_2D, TEXTURE_MAX_LEVEL, texture.vector.size() - 1 );
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )
