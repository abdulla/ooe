/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <jpeglib.h>

#include "foundation/executable/environment.hpp"
#include "foundation/image/jpeg.hpp"
#include "foundation/io/file.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

//--- jpeg_io --------------------------------------------------------------------------------------
struct jpeg_io
{
    ooe::file file;
    scoped_array< u8 > data;

    jpeg_io( const descriptor& desc )
        : file( desc ), data( new u8[ executable::static_page_size ] )
    {
    }
};

//--- error manager functions ----------------------------------------------------------------------
void jpeg_error( jpeg_common_struct* common )
{
    c8 string[ JMSG_LENGTH_MAX ];
    common->err->format_message( common, string );
    throw error::runtime( "jpeg: " ) << string;
}

void jpeg_warning( jpeg_common_struct* common )
{
    c8 string[ JMSG_LENGTH_MAX ];
    common->err->format_message( common, string );
    OOE_CONSOLE( "jpeg: " << string );
}

//--- source manager functions ---------------------------------------------------------------------
void jpeg_init( jpeg_decompress_struct* read_struct )
{
    read_struct->src->bytes_in_buffer = 0;
}

s32 jpeg_fill( jpeg_decompress_struct* read_struct )
{
    jpeg_io& io = *static_cast< jpeg_io* >( read_struct->client_data );
    read_struct->src->bytes_in_buffer = io.file.read( io.data, executable::static_page_size );
    read_struct->src->next_input_byte = io.data;
    return true;
}

void jpeg_term( jpeg_decompress_struct* )
{
}

void jpeg_skip( jpeg_decompress_struct* read_struct, sp_t size )
{
    if ( size < sp_t( read_struct->src->bytes_in_buffer ) )
    {
        read_struct->src->bytes_in_buffer -= size;
        read_struct->src->next_input_byte += size;
    }
    else
    {
        size -= read_struct->src->bytes_in_buffer;
        read_struct->src->bytes_in_buffer = 0;
        static_cast< jpeg_io* >( read_struct->client_data )->file.seek( size );
    }
}

//--- target manager functions ---------------------------------------------------------------------
void jpeg_init( jpeg_compress_struct* write_struct )
{
    write_struct->dest->free_in_buffer = executable::static_page_size;
    write_struct->dest->next_output_byte =
        static_cast< jpeg_io* >( write_struct->client_data )->data;
}

void jpeg_write( jpeg_compress_struct* write_struct, u32 size )
{
    jpeg_io& io = *static_cast< jpeg_io* >( write_struct->client_data );

    if ( io.file.write( io.data, size ) != size )
        throw error::runtime( "jpeg: " ) << "Unable to write";
}

void jpeg_term( jpeg_compress_struct* write_struct )
{
    jpeg_write( write_struct, executable::static_page_size - write_struct->dest->free_in_buffer );
}

s32 jpeg_empty( jpeg_compress_struct* write_struct )
{
    jpeg_write( write_struct, executable::static_page_size );
    jpeg_init( write_struct );
    return true;
}

//--- image format functions -----------------------------------------------------------------------
image_format::type jpeg_image_format( J_COLOR_SPACE colour_space )
{
    switch ( colour_space )
    {
    case JCS_GRAYSCALE:
        return image_format::y_u8;

    case JCS_RGB:
        return image_format::rgb_u8;

    default:
        throw error::runtime( "jpeg: " ) << "Unsupported colour type: " << colour_space;
    }
}

J_COLOR_SPACE jpeg_colour_space( image_format::type format )
{
    switch ( format )
    {
    case image_format::y_u8:
        return JCS_GRAYSCALE;

    case image_format::rgb_u8:
        return JCS_RGB;

    default:
        throw error::runtime( "jpeg: " ) << "Unsupported image format: " << format;
    }
}

image jpeg_swap_br( const image& in )
{
    if ( in.format != image_format::bgr_u8 )
        return in;

    image out( in.width, in.height, image_format::rgb_u8 );
    u8* x = in.as< u8 >();
    u8* y = out.as< u8 >();

    for ( u8* end = x + byte_size( in ); x != end; x += 3, y += 3 )
    {
        y[ 0 ] = x[ 2 ];
        y[ 1 ] = x[ 1 ];
        y[ 2 ] = x[ 0 ];
    }

    return out;
}

//--- jpeg_manager ---------------------------------------------------------------------------------
struct jpeg_manager
{
    jpeg_error_mgr error;

    jpeg_manager( void )
        : error()
    {
        jpeg_std_error( &error );
        error.error_exit = jpeg_error;
        error.output_message = jpeg_warning;
    }
};

//--- jpeg_read_state_base -------------------------------------------------------------------------
struct jpeg_read_state_base
{
    jpeg_manager manager;
    jpeg_decompress_struct decompress;

    jpeg_read_state_base( void )
        : manager(), decompress()
    {
        decompress.err = &manager.error;
        jpeg_create_decompress( &decompress );
    }

    ~jpeg_read_state_base( void )
    {
        jpeg_destroy_decompress( &decompress );
    }
};

//--- jpeg_read_state ------------------------------------------------------------------------------
struct jpeg_read_state
    : public jpeg_read_state_base
{
    jpeg_io io;
    jpeg_source_mgr source;

    jpeg_read_state( const descriptor& desc )
        : jpeg_read_state_base(), io( desc ), source()
    {
        source.init_source = jpeg_init;
        source.fill_input_buffer = jpeg_fill;
        source.skip_input_data = jpeg_skip;
        source.resync_to_restart = jpeg_resync_to_restart;
        source.term_source = jpeg_term;

        decompress.client_data = &io;
        decompress.src = &source;
        jpeg_read_header( &decompress, true );
        jpeg_start_decompress( &decompress );
    }

    ~jpeg_read_state( void )
    {
        jpeg_finish_decompress( &decompress );
    }

    image_metadata metadata( void ) const
    {
        return image_metadata( decompress.output_width, decompress.output_height,
            jpeg_image_format( decompress.out_color_space ) );
    }
};

//--- jpeg_write_state_base ------------------------------------------------------------------------
struct jpeg_write_state_base
{
    jpeg_manager manager;
    jpeg_compress_struct compress;

    jpeg_write_state_base( void )
        : manager(), compress()
    {
        compress.err = &manager.error;
        jpeg_create_compress( &compress );
    }

    ~jpeg_write_state_base( void )
    {
        jpeg_destroy_compress( &compress );
    }
};

//--- jpeg_write_state -----------------------------------------------------------------------------
struct jpeg_write_state
    : public jpeg_write_state_base
{
    jpeg_io io;
    jpeg_destination_mgr target;

    jpeg_write_state( const descriptor& desc, const image& image )
        : jpeg_write_state_base(), io( desc ), target()
    {
        target.init_destination = jpeg_init;
        target.empty_output_buffer = jpeg_empty;
        target.term_destination = jpeg_term;

        compress.image_width = image.width;
        compress.image_height = image.height;
        compress.input_components = subpixels( image );
        compress.in_color_space = jpeg_colour_space( image.format );

        compress.client_data = &io;
        compress.dest = &target;
        jpeg_set_defaults( &compress );
        jpeg_set_quality( &compress, 90, true );
        jpeg_start_compress( &compress, true );
    }

    ~jpeg_write_state( void )
    {
        jpeg_finish_compress( &compress );
    }
};

//--- jpeg_reader ----------------------------------------------------------------------------------
struct jpeg_reader
    : private jpeg_read_state, public image_reader
{
    jpeg_reader( const descriptor& desc )
        : jpeg_read_state( desc ), image_reader( metadata() )
    {
    }

    virtual bool decode_row( void )
    {
        if ( y == height )
            return false;

        u8* pointer = row.get();
        jpeg_read_scanlines( &decompress, &pointer, 1 );
        x = 0;
        ++y;
        return true;
    }
};

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( jpeg ) )

//--- jpeg -----------------------------------------------------------------------------------------
reader_ptr open( const descriptor& desc )
{
    return new jpeg_reader( desc );
}

image decode( const descriptor& desc )
{
    jpeg_read_state state( desc );
    image_metadata metadata = state.metadata();
    image image( metadata.width, metadata.height, metadata.format );
    u8* pointer = image.as< u8 >();
    up_t row_size = ooe::row_size( image );

    for ( u32 i = 0; i != image.height; ++i, pointer += row_size )
        jpeg_read_scanlines( &state.decompress, &pointer, 1 );

    return image;
}

void encode( const image& in, const descriptor& desc )
{
    image image = jpeg_swap_br( in );
    jpeg_write_state state( desc, image );
    u8* pointer = image.as< u8 >();
    up_t row_size = ooe::row_size( image );

    for ( u32 i = 0; i != image.height; ++i, pointer += row_size )
        jpeg_write_scanlines( &state.compress, &pointer, 1 );
}

OOE_NAMESPACE_END( ( ooe )( jpeg ) )
