/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <jpeglib.h>

#include "foundation/executable/environment.hpp"
#include "foundation/image/jpeg.hpp"
#include "foundation/io/file.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/utility/scoped.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

typedef u8 buffer_type[ executable::static_page_size ];
typedef tuple< file*, u8* > tuple_type;

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
    tuple_type& tuple = *static_cast< tuple_type* >( read_struct->client_data );
    read_struct->src->bytes_in_buffer = tuple._0->read( tuple._1, sizeof( buffer_type ) );
    read_struct->src->next_input_byte = tuple._1;
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
        static_cast< tuple_type* >( read_struct->client_data )->_0->seek( size );
    }
}

//--- target manager functions ---------------------------------------------------------------------
void jpeg_init( jpeg_compress_struct* write_struct )
{
    tuple_type& tuple = *static_cast< tuple_type* >( write_struct->client_data );
    write_struct->dest->free_in_buffer = sizeof( buffer_type );
    write_struct->dest->next_output_byte = tuple._1;
}

void jpeg_write( jpeg_compress_struct* write_struct, up_t size )
{
    tuple_type& tuple = *static_cast< tuple_type* >( write_struct->client_data );

    if ( tuple._0->write( tuple._1, size ) != size )
        throw error::runtime( "jpeg: " ) << "Unable to write";
}

void jpeg_term( jpeg_compress_struct* write_struct )
{
    jpeg_write( write_struct, sizeof( buffer_type ) - write_struct->dest->free_in_buffer );
}

s32 jpeg_empty( jpeg_compress_struct* write_struct )
{
    jpeg_term( write_struct );
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

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( jpeg ) )

//--- jpeg -----------------------------------------------------------------------------------------
image decode( const descriptor& desc )
{
    jpeg_decompress_struct read_struct;

    jpeg_error_mgr error_struct;
    read_struct.err = jpeg_std_error( &error_struct );
    error_struct.error_exit = jpeg_error;
    error_struct.output_message = jpeg_warning;

    file file( desc );
    buffer_type buffer;
    tuple_type tuple( &file, buffer );
    read_struct.client_data = &tuple;

    jpeg_create_decompress( &read_struct );
    scoped< void ( jpeg_decompress_struct* ) >
        scoped_read( jpeg_destroy_decompress, &read_struct );

    jpeg_source_mgr source_struct;
    source_struct.init_source = jpeg_init;
    source_struct.fill_input_buffer = jpeg_fill;
    source_struct.skip_input_data = jpeg_skip;
    source_struct.resync_to_restart = jpeg_resync_to_restart;
    source_struct.term_source = jpeg_term;
    read_struct.src = &source_struct;

    jpeg_read_header( &read_struct, true );
    jpeg_start_decompress( &read_struct );

    image_format::type type = jpeg_image_format( read_struct.out_color_space );
    image image( read_struct.output_width, read_struct.output_height, type );
    u8* pointer = image.as< u8 >();
    up_t row_size = ooe::row_size( image );

    for ( u32 i = 0; i != image.height; ++i, pointer += row_size )
        jpeg_read_scanlines( &read_struct, &pointer, 1 );

    jpeg_finish_decompress( &read_struct );
    return image;
}

void encode( const image& in, const descriptor& desc )
{
    jpeg_compress_struct write_struct;

    jpeg_error_mgr error_struct;
    write_struct.err = jpeg_std_error( &error_struct );
    error_struct.error_exit = jpeg_error;
    error_struct.output_message = jpeg_warning;

    file file( desc );
    buffer_type buffer;
    tuple_type tuple( &file, buffer );
    write_struct.client_data = &tuple;

    image image = jpeg_swap_br( in );
    jpeg_create_compress( &write_struct );
    scoped< void ( jpeg_compress_struct* ) > scoped( jpeg_destroy_compress, &write_struct );
    write_struct.image_width = image.width;
    write_struct.image_height = image.height;
    write_struct.input_components = subpixels( image );
    write_struct.in_color_space = jpeg_colour_space( image.format );

    jpeg_set_defaults( &write_struct );
    jpeg_set_quality( &write_struct, 80, true );

    jpeg_destination_mgr target_struct;
    target_struct.init_destination = jpeg_init;
    target_struct.empty_output_buffer = jpeg_empty;
    target_struct.term_destination = jpeg_term;
    write_struct.dest = &target_struct;

    jpeg_start_compress( &write_struct, true );
    u8* pointer = image.as< u8 >();
    up_t row_size = ooe::row_size( image );

    for ( u32 i = 0; i != image.height; ++i, pointer += row_size )
        jpeg_write_scanlines( &write_struct, &pointer, 1 );

    jpeg_finish_compress( &write_struct );
}

OOE_NAMESPACE_END( ( ooe )( jpeg ) )
