/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "component/ui/convert_source.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

image noop( const image& image )
{
    return image;
}

image rgb_u8_to_rgba_u8( const image& in )
{
    if ( in.format != image_format::rgb_u8 )
        throw error::runtime( "convert_source: " ) <<
            "Image format " << in.format << " != " << image_format::rgb_u8;

    image out( in.width, in.height, image_format::rgba_u8 );
    u8* j = out.as< u8 >();

    for ( u8* i = in.as< u8 >(), * end = i + byte_size( in ); i != end; i += 3, j += 4 )
    {
        j[ 0 ] = i[ 0 ];
        j[ 1 ] = i[ 1 ];
        j[ 2 ] = i[ 2 ];
        j[ 3 ] = 255;
    }

    return out;
}

convert_source::function_type source_rgb_u8( image_format::type target )
{
    switch ( target )
    {
    case image_format::rgba_u8:
        return rgb_u8_to_rgba_u8;

    default:
        throw error::runtime( "convert_source: " ) << "Unsupported target image format: " << target;
    }
}

convert_source::function_type converter( image_format::type source, image_format::type target )
{
    if ( source == target )
        return noop;

    switch ( source )
    {
    case image_format::rgb_u8:
        return source_rgb_u8( target );

    default:
        throw error::runtime( "convert_source: " ) << "Unsupported source image format: " << source;
    }
}

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- convert_source -------------------------------------------------------------------------------
convert_source::convert_source( physical_source& source_, image_format::type type )
    : source( source_ ), format_( type ), function( converter( source.format(), format_ ) )
{
}

convert_source::~convert_source( void )
{
}

u32 convert_source::size( void ) const
{
    return source.size();
}

u16 convert_source::page_size( void ) const
{
    return source.page_size();
}

image_format::type convert_source::format( void ) const
{
    return format_;
}

image convert_source::read( const pyramid_index& index )
{
    return function( source.read( index ) );
}

OOE_NAMESPACE_END( ( ooe ) )
