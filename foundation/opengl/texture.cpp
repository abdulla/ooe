/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl/symbol.hpp"
#include "foundation/opengl/texture.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/utility/miscellany.hpp"
#include "foundation/utility/tuple.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe )( opengl ) )

typedef tuple< u32, u32 > compressed_tuple;
typedef tuple< u32, u32, u32 > uncompressed_tuple;

//--- compressed_format ----------------------------------------------------------------------------
compressed_tuple compressed_format( const image_metadata& metadata )
{
    u32 size = ( ( metadata.width + 3 ) / 4 ) * ( ( metadata.height + 3 ) / 4 );

    switch ( metadata.format )
    {
    //--- dxt ------------------------------------------------------------------
    case image_format::rgba_dxt1:
        return compressed_tuple( COMPRESSED_RGBA_S3TC_DXT1, size * 8 );

    case image_format::rgba_dxt3:
        return compressed_tuple( COMPRESSED_RGBA_S3TC_DXT3, size * 16 );

    case image_format::rgba_dxt5:
        return compressed_tuple( COMPRESSED_RGBA_S3TC_DXT5, size * 16 );

    //--------------------------------------------------------------------------
    default:
        throw error::runtime( "opengl::texture: " ) <<
            "Unknown compressed image format: " << metadata.format;
    }
}

//--- uncompressed_format --------------------------------------------------------------------------
uncompressed_tuple uncompressed_format( image_format::type format )
{
    switch ( format )
    {
    case image_format::bgr_u8:
        return uncompressed_tuple( RGB8, BGR, UNSIGNED_BYTE );

    case image_format::bgra_u8:
        return uncompressed_tuple( RGBA8, BGRA, UNSIGNED_BYTE );

    //--- u8 -------------------------------------------------------------------
    case image_format::rgb_u8:
        return uncompressed_tuple( RGB8, RGB, UNSIGNED_BYTE );

    case image_format::rgba_u8:
        return uncompressed_tuple( RGBA8, RGBA, UNSIGNED_BYTE );

    case image_format::y_u8:
        return uncompressed_tuple( LUMINANCE8, LUMINANCE, UNSIGNED_BYTE );

    case image_format::ya_u8:
        return uncompressed_tuple( LUMINANCE8_ALPHA8, LUMINANCE_ALPHA, UNSIGNED_BYTE );

    case image_format::a_u8:
        return uncompressed_tuple( ALPHA8, ALPHA, UNSIGNED_BYTE );

    case image_format::r_u8:
        return uncompressed_tuple( R8, RED, UNSIGNED_BYTE );

    case image_format::rg_u8:
        return uncompressed_tuple( RG8, RG, UNSIGNED_BYTE );

    //--- s16 ------------------------------------------------------------------
    case image_format::rgb_s16:
        return uncompressed_tuple( RGB16I, RGB_INTEGER, SHORT );

    case image_format::rgba_s16:
        return uncompressed_tuple( RGBA16I, RGBA_INTEGER, SHORT );

    case image_format::y_s16:
        return uncompressed_tuple( LUMINANCE16I, LUMINANCE_INTEGER, SHORT );

    case image_format::ya_s16:
        return uncompressed_tuple( LUMINANCE_ALPHA16I, LUMINANCE_ALPHA_INTEGER, SHORT );

    case image_format::a_s16:
        return uncompressed_tuple( ALPHA16I, ALPHA_INTEGER, SHORT );

    case image_format::r_s16:
        return uncompressed_tuple( R16I, RED_INTEGER, SHORT );

    case image_format::rg_s16:
        return uncompressed_tuple( RG16I, RG_INTEGER, SHORT );

    //--- f16 ------------------------------------------------------------------
    case image_format::rgb_f16:
        return uncompressed_tuple( RGB16F, RGB, HALF_FLOAT );

    case image_format::rgba_f16:
        return uncompressed_tuple( RGBA16F, RGBA, HALF_FLOAT );

    case image_format::y_f16:
        return uncompressed_tuple( LUMINANCE16F, LUMINANCE, HALF_FLOAT );

    case image_format::ya_f16:
        return uncompressed_tuple( LUMINANCE_ALPHA16F, LUMINANCE_ALPHA, HALF_FLOAT );

    case image_format::a_f16:
        return uncompressed_tuple( ALPHA16F, ALPHA, HALF_FLOAT );

    case image_format::r_f16:
        return uncompressed_tuple( R16F, RED, HALF_FLOAT );

    case image_format::rg_f16:
        return uncompressed_tuple( RG16F, RG, HALF_FLOAT );

    //--- f32 ------------------------------------------------------------------
    case image_format::rgb_f32:
        return uncompressed_tuple( RGB32F, RGB, FLOAT );

    case image_format::rgba_f32:
        return uncompressed_tuple( RGBA32F, RGBA, FLOAT );

    case image_format::y_f32:
        return uncompressed_tuple( LUMINANCE32F, LUMINANCE, FLOAT );

    case image_format::ya_f32:
        return uncompressed_tuple( LUMINANCE_ALPHA16F, LUMINANCE_ALPHA, FLOAT );

    case image_format::a_f32:
        return uncompressed_tuple( ALPHA32F, ALPHA, FLOAT );

    case image_format::r_f32:
        return uncompressed_tuple( R32F, RED, FLOAT );

    case image_format::rg_f32:
        return uncompressed_tuple( RG32F, RG, FLOAT );

    //--------------------------------------------------------------------------
    default:
        throw error::runtime( "opengl::texture: " ) <<
            "Unknown uncompressed image format: " << format;
    }
}

//--- set_filter -----------------------------------------------------------------------------------
void set_filter( u32 target, ooe::texture::type filter )
{
    u32 min_filter;
    u32 mag_filter;

    switch ( filter )
    {
    case ooe::texture::nearest:
        min_filter = NEAREST_MIPMAP_NEAREST;
        mag_filter = NEAREST;
        break;

    case ooe::texture::linear:
        min_filter = LINEAR_MIPMAP_LINEAR;
        mag_filter = LINEAR;
        break;

    default:
        throw error::runtime( "opengl::texture: " ) << "Unknown filter type: " << filter;
    }

    TexParameteri( target, TEXTURE_MIN_FILTER, min_filter );
    TexParameteri( target, TEXTURE_MAG_FILTER, mag_filter );
    TexParameteri( target, TEXTURE_WRAP_S, CLAMP_TO_EDGE );
    TexParameteri( target, TEXTURE_WRAP_T, CLAMP_TO_EDGE );
}

//--- set_levels -----------------------------------------------------------------------------------
void set_levels( u32 target, bool generate_mipmap, u8 levels )
{
    if ( generate_mipmap )
        GenerateMipmap( target );
    else
        TexParameteri( target, TEXTURE_MAX_LEVEL, levels ? levels - 1 : 0 );
}

OOE_ANONYMOUS_END( ( ooe )( opengl ) )

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- texture_id -----------------------------------------------------------------------------------
texture_id::texture_id( void )
    : id()
{
    GenTextures( 1, const_cast< u32* >( &id ) );
}

texture_id::~texture_id( void )
{
    DeleteTextures( 1, &id );
}

//--- compressed_id --------------------------------------------------------------------------------
compressed_id::compressed_id( const image_metadata& metadata )
    : internal(), size()
{
    compressed_tuple tuple = compressed_format( metadata );
    const_cast< u32& >( internal ) = tuple._0;
    const_cast< u32& >( size ) = tuple._1;
}

//--- uncompressed_id ------------------------------------------------------------------------------
uncompressed_id::uncompressed_id( image_format::type format )
    : external(), data_type()
{
    uncompressed_tuple tuple = uncompressed_format( format );
    const_cast< u32& >( external ) = tuple._1;
    const_cast< u32& >( data_type ) = tuple._2;
}

//--- texture --------------------------------------------------------------------------------------
texture::texture( const image_pyramid& pyramid, texture::type filter, bool generate_mipmap_ )
    : texture_id(), image_metadata( pyramid ), levels( pyramid.size() ),
    generate_mipmap( generate_mipmap_ )
{
    BindTexture( TEXTURE_2D, id );
    set_filter( TEXTURE_2D, filter );
    set_levels( TEXTURE_2D, generate_mipmap, levels );
}

void texture::check( const image& image, u32 x, u32 y, u8 level ) const
{
    u32 w = width >> level;
    u32 h = height >> level;

    if ( level && generate_mipmap )
        throw error::runtime( "opengl::texture: " ) <<
            "Mipmap level " << level << " should be 0, if generate mipmap is set";
    else if ( level && level >= levels )
        throw error::runtime( "opengl::texture: " ) <<
            "Mipmap level " << level << " > maximum level " << levels - 1;
    else if ( image.width + x > w || image.height + y > h )
        throw error::runtime( "opengl::texture: " ) <<
            "Image size " << image.width << 'x' << image.height << " and offset " << x << ", " <<
            y << " is outside of texture size " << w << 'x' << h << " at level " << level;
    else if ( image.format != format )
        throw error::runtime( "opengl::texture: " ) <<
            "Image format " << image.format << " != texture format " << format;
}

//--- compressed_texture ---------------------------------------------------------------------------
compressed_texture::compressed_texture
    ( const image_pyramid& pyramid, texture::type filter, bool generate_mipmap_ )
    : texture( pyramid, filter, generate_mipmap_ ), compressed_id( pyramid )
{
    for ( u8 i = 0; i != levels; ++i )
        CompressedTexImage2D( TEXTURE_2D, i, internal, width >> i, height >> i, 0, size,
            pyramid[ i ] );

    if ( !levels )
        CompressedTexImage2D( TEXTURE_2D, 0, internal, width, height, 0, size, 0 );
}

void compressed_texture::write( const image& image, u32 x, u32 y, u8 level )
{
    check( image, x, y, level );
    BindTexture( TEXTURE_2D, id );
    CompressedTexSubImage2D( TEXTURE_2D, level, x, y, image.width, image.height, internal, size,
        image.get() );

    if ( generate_mipmap )
        GenerateMipmap( TEXTURE_2D );
}

//--- uncompressed_texture -------------------------------------------------------------------------
uncompressed_texture::uncompressed_texture
    ( const image_pyramid& pyramid, texture::type filter, bool generate_mipmap_ )
    : texture( pyramid, filter, generate_mipmap_ ), uncompressed_id( format )
{
    u32 internal = uncompressed_format( format )._0;

    for ( u8 i = 0; i != levels; ++i )
        TexImage2D( TEXTURE_2D, i, internal, width >> i, height >> i, 0, external, data_type,
            pyramid[ i ] );

    if ( !levels )
        TexImage2D( TEXTURE_2D, 0, internal, width, height, 0, external, data_type, 0 );
}

void uncompressed_texture::write( const image& image, u32 x, u32 y, u8 level )
{
    check( image, x, y, level );
    BindTexture( TEXTURE_2D, id );
    TexSubImage2D( TEXTURE_2D, level, x, y, image.width, image.height, external, data_type,
        image.get() );

    if ( generate_mipmap )
        GenerateMipmap( TEXTURE_2D );
}

//--- texture_array --------------------------------------------------------------------------------
texture_array::texture_array( const image_metadata& metadata_, u32 depth_ )
    : texture_id(), image_metadata( metadata_ ), depth( depth_ )
{
    BindTexture( TEXTURE_2D_ARRAY, id );
    set_filter( TEXTURE_2D_ARRAY, texture::linear );
    set_levels( TEXTURE_2D_ARRAY, false, 1 );
}

void texture_array::check( const image& image, u32 x, u32 y, u32 index ) const
{
    if ( index >= depth )
        throw error::runtime( "opengl::texture_array: " ) <<
            "Index " << index << " is out of range";
    else if ( image.width + x > width || image.height + y > height )
        throw error::runtime( "opengl::texture: " ) <<
            "Image size " << image.width << 'x' << image.height << " and offset " << x << ", " <<
            y << " is outside of texture size " << width << 'x' << height;
    else if ( image.format != format )
        throw error::runtime( "opengl::texture: " ) <<
            "Image format " << image.format << " != texture format " << format;
}

//--- compressed_texture_array ---------------------------------------------------------------------
compressed_texture_array::compressed_texture_array( const image_metadata& metadata_, u32 depth_ )
    : texture_array( metadata_, depth_ ), compressed_id( metadata_ )
{
    CompressedTexImage3D( TEXTURE_2D_ARRAY, 0, internal, width, height, depth, 0, size * depth, 0 );
}

void compressed_texture_array::write( const image& image, u32 x, u32 y, u32 index )
{
    check( image, x, y, index );
    BindTexture( TEXTURE_2D_ARRAY, id );
    CompressedTexSubImage3D( TEXTURE_2D_ARRAY, 0, x, y, index, image.width, image.height, 1,
        internal, size, image.get() );
}

//--- uncompressed_texture_array -------------------------------------------------------------------
uncompressed_texture_array::
    uncompressed_texture_array( const image_metadata& metadata_, u32 depth_ )
    : texture_array( metadata_, depth_ ), uncompressed_id( format )
{
    u32 internal = uncompressed_format( format )._0;
    TexImage3D( TEXTURE_2D_ARRAY, 0, internal, width, height, depth, 0, external, data_type, 0 );
}

void uncompressed_texture_array::write( const image& image, u32 x, u32 y, u32 index )
{
    check( image, x, y, index );
    BindTexture( TEXTURE_2D_ARRAY, id );
    TexSubImage3D( TEXTURE_2D_ARRAY, 0, x, y, index, image.width, image.height, 1, external,
        data_type, image.get() );
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )
