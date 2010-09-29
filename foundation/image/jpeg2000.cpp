/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <openjpeg.h>

#include "foundation/image/jpeg2000.hpp"
#include "foundation/io/file.hpp"
#include "foundation/io/memory.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/utility/scoped.hpp"

namespace
{
    using namespace ooe;

    void jpeg2000_warning( const c8* message, void* )
    {
        OOE_WARNING( "jpeg2000", "Warning: " << message );
    }

    image::type jpeg2000_image_type( OPJ_COLOR_SPACE colour_space, u8 channels )
    {
        switch ( colour_space )
        {
        case CLRSPC_GRAY:
            if ( channels == 1 )
                return image::y_u8;
            else if ( channels == 2 )
                return image::ya_u8;
            else
                break;

        case CLRSPC_SRGB:
            if ( channels == 3 )
                return image::rgb_u8;
            else if ( channels == 4 )
                return image::rgba_u8;
            else
        default:
                break;
        }

        throw error::runtime( "jpeg2000: " ) << "Unsupported colour type";
    }

    OPJ_COLOR_SPACE jpeg2000_colour_space( u8 type )
    {
        switch ( type )
        {
        case image::y_u8:
        case image::ya_u8:
            return CLRSPC_GRAY;

        case image::rgb_u8:
        case image::rgba_u8:
            return CLRSPC_SRGB;

        default:
            throw error::runtime( "jpeg2000: " ) << "Unsupported image format";
        }
    }
}

namespace ooe
{
//--- jpeg2000 -----------------------------------------------------------------
    uncompressed_image jpeg2000::decode( const descriptor& desc )
    {
        memory memory( desc );
        return jpeg2000::decode( memory.get(), memory.size() );
    }

    uncompressed_image jpeg2000::decode( const void* data, up_t size )
    {
        scoped_ptr< opj_dparameters_t > parameters( new opj_dparameters_t );
        opj_set_default_decoder_parameters( parameters );
        opj_dinfo_t* read_info = opj_create_decompress( CODEC_JP2 );

        if ( !read_info )
            throw error::runtime( "jpeg2000: " ) << "Unable to create decompression info";

        scoped< void ( opj_dinfo_t* ) > scoped_read( opj_destroy_decompress, read_info );
        opj_common_struct_t* common_info = reinterpret_cast< opj_common_struct_t* >( read_info );

        opj_event_mgr_t events = { jpeg2000_warning, jpeg2000_warning, 0 };
        opj_set_event_mgr( common_info, &events, 0 );
        opj_setup_decoder( read_info, parameters );
        opj_cio_t* io_info = opj_cio_open( common_info, ( u8* )data, size );

        if ( !io_info )
            throw error::runtime( "jpeg2000: " ) << "Unable to create input-output info";

        scoped< void ( opj_cio_t* ) > scoped_io( opj_cio_close, io_info );
        opj_image_t* image = opj_decode( read_info, io_info );

        if ( !image )
            throw error::runtime( "jpeg2000: " ) << "Unable to decode image";

        scoped< void ( opj_image_t* ) > scoped_image( opj_image_destroy, image );
        u32 width = image->x1 - image->x0;
        u32 height = image->y1 - image->y0;
        u32 length = width * height;
        image::type type = jpeg2000_image_type( image->color_space, image->numcomps );
        uncompressed_image out( width, height, type );

        for ( s32 i = 0; i != image->numcomps; ++i )
        {
            opj_image_comp_t* comp = image->comps + i;

            if ( comp->prec != 8 )
                throw error::runtime( "jpeg2000: " ) << "Unsupported depth: " << comp->prec;

            skip_out( comp->data, comp->data + length, out.as< u8 >() + i, image->numcomps );
        }

        return out;
    }

    void jpeg2000::encode( const uncompressed_image& in, const descriptor& desc )
    {
        OPJ_COLOR_SPACE colour_space = jpeg2000_colour_space( in.format );
        u8 channels = in.channels();
        opj_image_cmptparm_t component[ 4 ];
        std::memset( component, 0, sizeof( component ) );

        for ( u8 i = 0; i != channels; ++i )
        {
            opj_image_cmptparm_t* cmptparm = component + i;
            cmptparm->prec = 8;
            cmptparm->bpp = 8;
            cmptparm->dx = 1;
            cmptparm->dy = 1;
            cmptparm->w = in.width;
            cmptparm->h = in.height;
        }

        //----------------------------------------------------------------------
        opj_image_t* image = opj_image_create( channels, component, colour_space );

        if ( !image )
            throw error::runtime( "jpeg2000: " ) << "Unable to create image";

        scoped< void ( opj_image_t* ) > scoped_image( opj_image_destroy, image );
        image->x0 = 0;
        image->y0 = 0;
        image->x1 = in.width;
        image->y1 = in.height;
        u8* begin = in.as< u8 >();
        u8* end = begin + in.byte_size();

        for ( u8 i = 0; i != channels; ++i )
            skip_in( begin + i, end + i, image->comps[ i ].data, channels );

        //----------------------------------------------------------------------
        scoped_ptr< opj_cparameters_t > parameters( new opj_cparameters_t );
        opj_set_default_encoder_parameters( parameters );
        parameters->tcp_rates[ 0 ] = 20.;
        parameters->tcp_numlayers = 1;
        parameters->tcp_mct = channels == 3;
        parameters->cp_disto_alloc = 1;
        opj_cinfo_t* write_info = opj_create_compress( CODEC_JP2 );

        if ( !write_info )
            throw error::runtime( "jpeg2000: " ) << "Unable to create compression info";

        scoped< void ( opj_cinfo_t* ) > scoped_write( opj_destroy_compress, write_info );
        opj_common_struct_t* common_info = reinterpret_cast< opj_common_struct_t* >( write_info );

        opj_event_mgr_t events = { jpeg2000_warning, jpeg2000_warning, 0 };
        opj_set_event_mgr( common_info, &events, 0 );
        opj_setup_encoder( write_info, parameters, image );
        opj_cio_t* io_info = opj_cio_open( common_info, 0, 0 );

        if ( !io_info )
            throw error::runtime( "jpeg2000: " ) << "Unable to create input-output info";

        scoped< void ( opj_cio_t* ) > scoped_io( opj_cio_close, io_info );

        if ( !opj_encode( write_info, io_info, image, 0 ) )
            throw error::runtime( "jpeg2000: " ) << "Unable to encode image";

        file file( desc );
        file.write( io_info->buffer, cio_tell( io_info ) );
    }
}
