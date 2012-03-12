/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "foundation/utility/error.hpp"
#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- map ------------------------------------------------------------------------------------------
map::map( void* data_, up_t size_ )
    : data( data_ ), size( size_ )
{
}

//--- block ----------------------------------------------------------------------------------------
void block::input( const std::string& name, s32 a )
{
    s32 data[] = { a };
    input( name, &data, 1 );
}

void block::input( const std::string& name, s32 a, s32 b )
{
    s32 data[] = { a, b };
    input( name, &data, 1 );
}

void block::input( const std::string& name, s32 a, s32 b, s32 c )
{
    s32 data[] = { a, b, c };
    input( name, &data, 1 );
}

void block::input( const std::string& name, s32 a, s32 b, s32 c, s32 d )
{
    s32 data[] = { a, b, c, d };
    input( name, &data, 1 );
}

void block::input( const std::string& name, f32 a )
{
    f32 data[] = { a };
    input( name, &data, 1 );
}

void block::input( const std::string& name, f32 a, f32 b )
{
    f32 data[] = { a, b };
    input( name, &data, 1 );
}

void block::input( const std::string& name, f32 a, f32 b, f32 c )
{
    f32 data[] = { a, b, c };
    input( name, &data, 1 );
}

void block::input( const std::string& name, f32 a, f32 b, f32 c, f32 d )
{
    f32 data[] = { a, b, c, d };
    input( name, &data, 1 );
}

//--- image_pyramid --------------------------------------------------------------------------------
image_pyramid::image_pyramid( const ooe::image& image_ )
    : image_metadata( image_ ), vector( 1, image_.ptr() )
{
}

image_pyramid::image_pyramid( const image_metadata& metadata )
    : image_metadata( metadata ), vector()
{
}

void image_pyramid::insert( const ooe::image& image_ )
{
    u32 level_width = width >> vector.size();
    u32 level_height = height >> vector.size();

    if ( image_.width != level_width || image_.height != level_height )
        throw error::runtime( "image_pyramid: " ) <<
            "Image size " << image_.width << 'x' << image_.height << " != " <<
            level_width << 'x' << level_height << " required for level " << vector.size();
    else if ( image_.format != format )
        throw error::runtime( "image_pyramid: " ) <<
            "Image format " << image_.format << " != " << format;

    vector.push_back( image_.ptr() );
}

ooe::image image_pyramid::image( u8 level ) const
{
    return ooe::image( width >> level, height >> level, format, vector[ level ] );
}

u8 image_pyramid::size( void ) const
{
    return vector.size();
}

image::data_ptr image_pyramid::operator []( u8 level ) const
{
    return vector[ level ];
}

OOE_NAMESPACE_END( ( ooe ) )
