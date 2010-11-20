/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <png.h>

#include "foundation/image/png.hpp"
#include "foundation/io/file.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

//--- png ------------------------------------------------------------------------------------------
void png_error_( png_struct*, const c8* string )
{
    throw error::runtime( "png: " ) << string;
}

void png_warning_( png_struct*, const c8* string )
{
    OOE_CONSOLE( "png: " << string );
}

void png_read( png_struct* png, u8* buffer, up_t size )
{
    static_cast< ooe::file* >( png_get_io_ptr( png ) )->read( buffer, size );
}

void png_write( png_struct* png, u8* buffer, up_t size )
{
    static_cast< ooe::file* >( png_get_io_ptr( png ) )->write( buffer, size );
}

void png_sync( png_struct* png )
{
    static_cast< ooe::file* >( png_get_io_ptr( png ) )->sync();
}

image_format::type png_image_format( u8 format )
{
    switch ( format )
    {
    case PNG_COLOR_TYPE_GRAY:
        return image_format::y_u8;

    case PNG_COLOR_TYPE_GRAY_ALPHA:
        return image_format::ya_u8;

    case PNG_COLOR_TYPE_RGB:
        return image_format::rgb_u8;

    case PNG_COLOR_TYPE_RGB_ALPHA:
        return image_format::rgba_u8;

    default:
        throw error::runtime( "png: " ) << "Unsupported colour type";
    }
}

u8 png_colour_type( png_struct* png, image_format::type format )
{
    switch ( format )
    {
    case image_format::y_u8:
        return PNG_COLOR_TYPE_GRAY;

    case image_format::ya_u8:
        return PNG_COLOR_TYPE_GRAY_ALPHA;

    case image_format::bgr_u8:
        png_set_bgr( png );
    case image_format::rgb_u8:
        return PNG_COLOR_TYPE_RGB;

    case image_format::bgra_u8:
        png_set_bgr( png );
    case image_format::rgba_u8:
        return PNG_COLOR_TYPE_RGB_ALPHA;

    default:
        throw error::runtime( "png: " ) << "Unsupported image format";
    }
}

//--- png_read_state -------------------------------------------------------------------------------
struct png_read_state
{
    ooe::file file;
    png_struct* png;

    png_read_state( const descriptor& desc )
        : file( desc ),
        png( png_create_read_struct( PNG_LIBPNG_VER_STRING, 0, png_error_, png_warning_ ) )
    {
        if ( !png )
            throw error::runtime( "png_reader: " ) << "Unable to create read structure";

        png_set_expand( png );
        png_set_packing( png );
        png_set_read_fn( png, &file, png_read );
    }

    png_read_state( png_read_state& state )
        : file( state.file ), png( state.png )
    {
        state.png = 0;
        png_set_read_fn( png, &file, png_read );
    }

    ~png_read_state( void )
    {
        png_destroy_read_struct( &png, 0, 0 );
    }
};

//--- png_write_state ------------------------------------------------------------------------------
struct png_write_state
{
    ooe::file file;
    png_struct* png;

    png_write_state( const descriptor& desc )
        : file( desc ),
        png( png_create_write_struct( PNG_LIBPNG_VER_STRING, 0, png_error_, png_warning_ ) )
    {
        if ( !png )
            throw error::runtime( "png: " ) << "Unable to create write structure";

        png_set_compression_level( png, Z_BEST_COMPRESSION );
        png_set_write_fn( png, &file, png_write, png_sync );
    }

    ~png_write_state( void )
    {
        png_destroy_write_struct( &png, 0 );
    }
};

//--- png_header -----------------------------------------------------------------------------------
struct png_header
{
    png_struct* png;
    png_info* info;

    png_header( const png_read_state& state )
        : png( state.png ), info( png_create_info_struct( png ) )
    {
        if ( !info )
            throw error::runtime( "png_reader: " ) << "Unable to create info structure";

        png_read_info( png, info );
        png_read_update_info( png, info );
    }

    png_header( const png_write_state& state, const image& image )
        : png( state.png ), info( png_create_info_struct( png ) )
    {
        if ( !info )
            throw error::runtime( "png_reader: " ) << "Unable to create info structure";

        png_set_IHDR( png, info, image.width, image.height, 8, png_colour_type( png, image.format ),
            PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT );
        png_write_info( png, info );
    }

    ~png_header( void )
    {
        png_destroy_info_struct( png, &info );
    }
};

//--- png_metadata ---------------------------------------------------------------------------------
struct png_metadata
    : public image_metadata
{
    png_metadata( const png_read_state& state, const png_header& header )
        : image_metadata( png_get_image_width( state.png, header.info ),
        png_get_image_height( state.png, header.info ),
        png_image_format( png_get_color_type( state.png, header.info ) ) )
    {
        u32 bit_depth = png_get_bit_depth( state.png, header.info );

        if ( bit_depth != 8 )
            throw error::runtime( "png_reader: " ) << "Unsupported bit depth " << bit_depth;
    }
};

//--- png_reader -----------------------------------------------------------------------------------
class png_reader
    : public image_reader
{
public:
    png_reader( const image_metadata& metadata, png_read_state& state_ )
        : image_reader( metadata ), state( state_ ), row( new u8[ row_size( *this ) ] ), x( width ),
        y( 0 )
    {
    }

    virtual ~png_reader( void )
    {
        if ( y )
            png_read_end( state.png, 0 );
    }

    virtual bool decode_row( void )
    {
        if ( y == height )
            return false;

        png_read_row( state.png, row, 0 );
        x = 0;
        ++y;
        return true;
    }

    virtual u32 read_pixels( void* buffer, u32 pixels )
    {
        u8 size = pixel_size( *this );
        pixels = std::min( width, x + pixels ) - x;
        std::memcpy( buffer, row + x * size, pixels * size );
        x += pixels;
        return pixels;
    }

private:
    png_read_state state;
    scoped_array< u8 > row;
    u32 x;
    u32 y;
};

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( png ) )

//--- png ------------------------------------------------------------------------------------------
reader_type open( const descriptor& desc )
{
    png_read_state state( desc );
    png_header header( state );
    png_metadata metadata( state, header );
    return new png_reader( metadata, state );
}

image decode( const descriptor& desc )
{
    png_read_state state( desc );
    png_header header( state );
    png_metadata metadata( state, header );
    image image( metadata.width, metadata.height, metadata.format );
    u8* row = image.as< u8 >();
    up_t row_size = ooe::row_size( image );

    for ( u32 i = 0; i != image.height; ++i, row += row_size )
        png_read_row( state.png, row, 0 );

    png_read_end( state.png, 0 );
    return image;
}

void encode( const image& image, const descriptor& desc )
{
    png_write_state state( desc );
    png_header header( state, image );
    u8* pointer = image.as< u8 >();
    up_t row_size = ooe::row_size( image );

    for ( u32 i = 0; i != image.height; ++i, pointer += row_size )
        png_write_row( state.png, pointer );

    png_write_end( state.png, 0 );
}

OOE_NAMESPACE_END( ( ooe )( png ) )
