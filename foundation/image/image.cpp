/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cstdlib>

#include "foundation/image/image.hpp"
#include "foundation/utility/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- image_metadata -------------------------------------------------------------------------------
image_metadata::image_metadata( u32 width_, u32 height_, image_format::type format_ )
    : width( width_ ), height( height_ ), format( format_ )
{
}

//--- image ----------------------------------------------------------------------------------------
image::image( u32 width_, u32 height_, image_format::type format_ )
    : image_metadata( width_, height_, format_ ), data( std::malloc( byte_size( *this ) ) )
{
}

image::image( u32 width_, u32 height_, image_format::type format_, const data_type& data_ )
    : image_metadata( width_, height_, format_ ), data( data_ )
{
}

//--- image_reader ---------------------------------------------------------------------------------
image_reader::image_reader( const image_metadata& metadata )
    : image_metadata( metadata )
{
}

//--------------------------------------------------------------------------------------------------
bool is_compressed( image_format::type format )
{
    return format >= image_format::type( image_format::compressed );
}


u8 subpixels( const image_metadata& metadata )
{
    switch ( metadata.format )
    {
    case image_format::y_u8:
    case image_format::y_s16:
    case image_format::y_f16:
    case image_format::y_f32:
    case image_format::a_u8:
    case image_format::a_s16:
    case image_format::a_f16:
    case image_format::a_f32:
    case image_format::r_u8:
    case image_format::r_s16:
    case image_format::r_f16:
    case image_format::r_f32:
        return 1;

    case image_format::ya_u8:
    case image_format::ya_s16:
    case image_format::ya_f16:
    case image_format::ya_f32:
    case image_format::rg_u8:
    case image_format::rg_s16:
    case image_format::rg_f16:
    case image_format::rg_f32:
        return 2;

    case image_format::bgr_u8:
    case image_format::rgb_u8:
    case image_format::rgb_s16:
    case image_format::rgb_f16:
    case image_format::rgb_f32:
        return 3;

    case image_format::bgra_u8:
    case image_format::rgba_u8:
    case image_format::rgba_s16:
    case image_format::rgba_f16:
    case image_format::rgba_f32:
    case image_format::rgba_dxt1:
    case image_format::rgba_dxt3:
    case image_format::rgba_dxt5:
        return 4;

    default:
        throw error::runtime( "subpixels: " ) << "Unknown image type: " << metadata.format;
    }
}

u8 subpixel_size( const image_metadata& metadata )
{
    switch ( metadata.format )
    {
    case image_format::bgr_u8:
    case image_format::bgra_u8:
    case image_format::rgb_u8:
    case image_format::rgba_u8:
    case image_format::y_u8:
    case image_format::ya_u8:
    case image_format::a_u8:
    case image_format::r_u8:
    case image_format::rg_u8:
        return 1;

    case image_format::rgb_s16:
    case image_format::rgba_s16:
    case image_format::y_s16:
    case image_format::ya_s16:
    case image_format::a_s16:
    case image_format::r_s16:
    case image_format::rg_s16:
    case image_format::rgb_f16:
    case image_format::rgba_f16:
    case image_format::y_f16:
    case image_format::ya_f16:
    case image_format::a_f16:
    case image_format::r_f16:
    case image_format::rg_f16:
        return 2;

    case image_format::rgb_f32:
    case image_format::rgba_f32:
    case image_format::y_f32:
    case image_format::ya_f32:
    case image_format::a_f32:
    case image_format::r_f32:
    case image_format::rg_f32:
        return 4;

    case image_format::rgba_dxt1:
    case image_format::rgba_dxt3:
    case image_format::rgba_dxt5:
        throw error::runtime( "subpixel_size: " ) <<
            "Can not use compressed image format: " << metadata.format;

    default:
        throw error::runtime( "subpixel_size: " ) << "Unknown image type: " << metadata.format;
    }
}

up_t pixels( const image_metadata& metadata )
{
    return metadata.width * metadata.height;
}

u8 pixel_size( const image_metadata& metadata )
{
    return subpixels( metadata ) * subpixel_size( metadata );
}

u8 block_size( const image_metadata& metadata )
{
    switch( metadata.format )
    {
    case image_format::bgr_u8:
    case image_format::bgra_u8:
    case image_format::rgb_u8:
    case image_format::rgba_u8:
    case image_format::y_u8:
    case image_format::ya_u8:
    case image_format::a_u8:
    case image_format::r_u8:
    case image_format::rg_u8:
    case image_format::rgb_s16:
    case image_format::rgba_s16:
    case image_format::y_s16:
    case image_format::ya_s16:
    case image_format::a_s16:
    case image_format::r_s16:
    case image_format::rg_s16:
    case image_format::rgb_f16:
    case image_format::rgba_f16:
    case image_format::y_f16:
    case image_format::ya_f16:
    case image_format::a_f16:
    case image_format::r_f16:
    case image_format::rg_f16:
    case image_format::rgb_f32:
    case image_format::rgba_f32:
    case image_format::y_f32:
    case image_format::ya_f32:
    case image_format::a_f32:
    case image_format::r_f32:
    case image_format::rg_f32:
        throw error::runtime( "block_size: " ) <<
            "Can not use uncompressed format: " << metadata.format;

    case image_format::rgba_dxt1:
        return 8;

    case image_format::rgba_dxt3:
    case image_format::rgba_dxt5:
        return 16;

    default:
        throw error::runtime( "block_size: " ) << "Unknown image type: " << metadata.format;
    }
}

up_t row_size( const image_metadata& metadata )
{
    return metadata.width * pixel_size( metadata );
}

up_t byte_size( const image_metadata& metadata )
{
    if ( is_compressed( metadata.format ) )
        return ( ( metadata.width + 3 ) / 4 ) * ( ( metadata.height + 3 ) / 4 ) *
            block_size( metadata );
    else
        return pixels( metadata ) * pixel_size( metadata );
}

OOE_NAMESPACE_END( ( ooe ) )
