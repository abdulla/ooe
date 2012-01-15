/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <squish.h>

#include "foundation/image/dds.hpp"
#include "foundation/io/file.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

struct OOE_PACKED dds_format
{
    enum
    {
        use_fourcc      = 0x00000004
    };

    u32 size;   // sizeof( dds_format ) = 32
    u32 flags;
    u32 fourcc;
    u32 rgb_bits;
    u32 r_mask;
    u32 g_mask;
    u32 b_mask;
    u32 a_mask;
};

struct OOE_PACKED dds_header
{
    enum
    {
        use_texture     = 0x00001007,
        use_linear_size = 0x00080000
    };

    u32 size;   // sizeof( dds_header ) = 124
    u32 flags;
    u32 height;
    u32 width;
    u32 linear_size;
    u32 depth;
    u32 mipmaps;
    u32 reserved_1[11];
    dds_format format;
    u32 surface_flags;
    u32 cubemap_flags;
    u32 reserved_2[ 3 ];
};

image_format::type dds_image_format( u32 fourcc )
{
    switch ( fourcc )
    {
    case OOE_FOURCC( 'D', 'X', 'T', '1' ):
        return image_format::rgba_dxt1;

    case OOE_FOURCC( 'D', 'X', 'T', '3' ):
        return image_format::rgba_dxt3;

    case OOE_FOURCC( 'D', 'X', 'T', '5' ):
        return image_format::rgba_dxt5;

    default:
        throw error::runtime( "dds: " ) << "Unsupported dxt version";
    }
}

u32 dds_image_fourcc( image_format::type format )
{
    switch ( format )
    {
    case image_format::rgba_dxt1:
        return OOE_FOURCC( 'D', 'X', 'T', '1' );

    case image_format::rgba_dxt3:
        return OOE_FOURCC( 'D', 'X', 'T', '3' );

    case image_format::rgba_dxt5:
        return OOE_FOURCC( 'D', 'X', 'T', '5' );

    default:
        throw error::runtime( "dds: " ) << "Unsupported image format: " << format;
    }
}

u32 dxt_version( image_format::type format )
{
    switch ( format )
    {
    case image_format::rgba_dxt1:
        return squish::kDxt1;

    case image_format::rgba_dxt3:
        return squish::kDxt3;

    case image_format::rgba_dxt5:
        return squish::kDxt5;

    default:
        throw error::runtime( "dxt: " ) << "Unsupported image format: " << format;
    }
}

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( dds ) )

//--- dds ------------------------------------------------------------------------------------------
image decode( const descriptor& desc )
{
    file file( desc );
    u32 fourcc;

    if ( file.read( &fourcc, sizeof( fourcc ) ) != sizeof( fourcc ) )
        throw error::runtime( "dds: " ) << "Unable to read fourcc";
    else if ( fourcc != OOE_FOURCC( 'D', 'D', 'S', ' ' ) )
        throw error::runtime( "dds: " ) << "Image is not a DirectDraw Surface";

    dds_header header;

    if ( file.read( &header, sizeof( header ) ) != sizeof( header ) )
        throw error::runtime( "dds: " ) << "Unable to read header";
    else if ( header.size != sizeof( header ) )
        throw error::runtime( "dds: " ) << "Incorrect header size " << header.size;
    else if ( header.format.size != sizeof( dds_format ) )
        throw error::runtime( "dds: " ) <<
            "Incorrect pixel format size " << header.format.size;
    else if ( ~header.flags & dds_header::use_texture )
        throw error::runtime( "dds: " ) << "Image is not 2-dimensional";
    else if ( ~header.format.flags & dds_format::use_fourcc )
        throw error::runtime( "dds: " ) << "Unsupported pixel format";

    image_format::type type = dds_image_format( header.format.fourcc );
    image image( header.width, header.height, type );
    up_t byte_size = ooe::byte_size( image );
    up_t size = file.size();

    // conditionally verify linear_size, needed to work around microsoft bug
    if ( header.flags & dds_header::use_linear_size && header.linear_size != byte_size )
        throw error::runtime( "dds: " ) <<
            "Linear size " << header.linear_size << " does not match image size " << byte_size;
    else if ( header.linear_size + sizeof( dds_header ) + 4 > size )
        throw error::runtime( "dds: " ) <<
            "Linear size " << header.linear_size << " is greater than file size " << size;

    file.read( image.get(), byte_size );
    return image;
}

void encode( const image& image, const descriptor& desc )
{
    file file( desc );
    u32 fourcc = OOE_FOURCC( 'D', 'D', 'S', ' ' );

    dds_header header;
    std::memset( &header, 0, sizeof( dds_header ) );
    header.size = sizeof( dds_header );
    header.flags = dds_header::use_texture | dds_header::use_linear_size;
    header.height = image.height;
    header.width = image.width;
    header.linear_size = byte_size( image );
    header.format.size = sizeof( dds_format );
    header.format.flags = dds_format::use_fourcc;
    header.format.fourcc = dds_image_fourcc( image.format );

    if ( file.write( &fourcc, sizeof( fourcc ) ) != sizeof( fourcc ) ||
        file.write( &header, sizeof( header ) )  != sizeof( header ) ||
        file.write( image.get(), header.linear_size ) != header.linear_size )
        throw error::runtime( "dds: " ) << "Unable to write image";
}

OOE_NAMESPACE_END( ( ooe )( dds ) )

OOE_NAMESPACE_BEGIN( ( ooe )( dxt ) )

//--- dxt ------------------------------------------------------------------------------------------
image decode( const image& in )
{
    image out( in.width, in.height, image_format::rgba_u8 );
    u32 flags = dxt_version( in.format );
    squish::DecompressImage( out.as< u8 >(), in.width, in.height, in.as< u8 >(), flags );
    return out;
}

image encode( const image& in, image_format::type format )
{
    image out( in.width, in.height, format );
    u32 flags = dxt_version( format ) | squish::kColourIterativeClusterFit;
    squish::CompressImage( in.as< u8 >(), in.width, in.height, out.as< u8 >(), flags );
    return out;
}

OOE_NAMESPACE_END( ( ooe )( dxt ) )
