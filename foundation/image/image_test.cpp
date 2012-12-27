/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <cstring>

#include <paths.h>

#include "foundation/image/dds.hpp"
#include "foundation/image/jpeg.hpp"
#include "foundation/image/jpeg2000.hpp"
#include "foundation/image/png.hpp"
#include "foundation/io/descriptor.hpp"
#include "foundation/io/directory.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

template< typename type >
    image make_image( type red, type green, type blue, image_format::type format )
{
    image image( 16, 16, format );
    u8 subpixels = ooe::subpixels( image );

    for ( type* i = image.as< type >(), * end = i + pixels( image ) * subpixels; i != end;
        i += subpixels )
    {
        i[ 0 ] = red;
        i[ 1 ] = green;
        i[ 2 ] = blue;

        if ( subpixels == 4 )
            i[ 3 ] = 255;
    }

    return image;
}

typedef unit::group< anonymous_t, anonymous_t, 6 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "image" );

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

OOE_TEST( 0 )
{
    std::cerr << "uncompressed_image";
    ooe::image image( 16, 16, image_format::rgba_u8 );

    OOE_CHECK( image.width == 16 );
    OOE_CHECK( image.height == 16 );
    OOE_CHECK( image.format == image_format::rgba_u8 );
    OOE_CHECK( image.get() );
    OOE_CHECK( subpixels( image ) == 4 );
    OOE_CHECK( subpixel_size( image ) == 1 );
    OOE_CHECK( pixels( image ) == 256 );
    OOE_CHECK( pixel_size( image ) == 4 );
    OOE_CHECK( row_size( image ) == 64 );
    OOE_CHECK( byte_size( image ) == 1024 );
}

OOE_TEST( 1 )
{
    std::cerr << "dxt";
    ooe::image image = make_image< u8 >( 255, 255, 255, image_format::rgba_u8 );

    ooe::image dxt1 = dxt::encode( image, image_format::rgba_dxt1 );
    ooe::image dxt1_uncompressed = dxt::decode( dxt1 );
    OOE_CHECK( block_size( dxt1 ) == 8 );
    OOE_CHECK( byte_size( dxt1 ) == 128 );

    ooe::image dxt3 = dxt::encode( image, image_format::rgba_dxt3 );
    ooe::image dxt3_uncompressed = dxt::decode( dxt3 );
    OOE_CHECK( block_size( dxt3 ) == 16 );
    OOE_CHECK( byte_size( dxt3 ) == 256 );

    ooe::image dxt5 = dxt::encode( image, image_format::rgba_dxt5 );
    ooe::image dxt5_uncompressed = dxt::decode( dxt5 );
    OOE_CHECK( block_size( dxt5 ) == 16 );
    OOE_CHECK( byte_size( dxt5 ) == 256 );
}

OOE_TEST( 2 )
{
    std::cerr << "dds";
    const c8 path[] = _PATH_TMP "ooe.test.dds";

    image_format::type format[] =
    {
        image_format::rgba_dxt1,
        image_format::rgba_dxt3,
        image_format::rgba_dxt5
    };

    image input[] =
    {
        make_image< u8 >( 255, 0, 0, image_format::rgba_u8 ),
        make_image< u8 >( 0, 255, 0, image_format::rgba_u8 ),
        make_image< u8 >( 0, 0, 255, image_format::rgba_u8 )
    };

    for ( u8 i = 0; i != 3; ++i )
    {
        for ( u8 j = 0; j != 3; ++j )
        {
            image dxt_in = dxt::encode( input[ j ], format[ i ] );
            dds::encode( dxt_in, descriptor( path, descriptor::write_new ) );
            image dxt_out = dds::decode( descriptor( path ) );
            image output = dxt::decode( dxt_out );

            OOE_CHECK( byte_size( dxt_in ) == byte_size( dxt_out ) ) << "dxt sizes do not match";
            OOE_CHECK( !std::memcmp( dxt_in.get(), dxt_out.get(), byte_size( dxt_in ) ) ) <<
                "dxt data does not match";

            OOE_CHECK( byte_size( input[ j ] ) == byte_size( output ) ) <<
                "decoded sizes do not match";
            OOE_CHECK( !std::memcmp( input[ j ].get(), output.get(), byte_size( output ) ) ) <<
                "decoded data does not match";
        }
    }

    erase( path );
}

OOE_TEST( 3 )
{
    std::cerr << "jpeg";
    const c8 path[] = _PATH_TMP "ooe.test.jpg";

    image input[] =
    {
        make_image< u8 >( 255, 0, 0, image_format::rgb_u8 ),
        make_image< u8 >( 0, 255, 0, image_format::rgb_u8 ),
        make_image< u8 >( 0, 0, 255, image_format::rgb_u8 )
    };

    // no test for data match due to minor differences caused by lossy compression
    for ( u8 i = 0; i != 3; ++i )
    {
        jpeg::encode( input[ i ], descriptor( path, descriptor::write_new ) );
        image output = jpeg::decode( descriptor( path ) );
        OOE_CHECK( input[ i ] == output ) << "decoded metadata does not match";
        OOE_CHECK( byte_size( input[ i ] ) == byte_size( output ) ) << "decoded sizes do not match";

        reader_ptr reader = jpeg::open( descriptor( path ) );
        OOE_CHECK( input[ i ] == *reader ) << "reader metadata does not match";
        OOE_CHECK( byte_size( input[ i ] ) == byte_size( *reader ) ) <<
            "reader size does not match";

        std::memset( output.get(), 0, byte_size( output ) );
        up_t row_size = ooe::row_size( *reader );
        u32 rows = 0;

        for ( u8* j = output.as< u8 >(); reader->decode_row(); ++rows, j += row_size )
            OOE_CHECK( reader->read_pixels( j, reader->width ) == reader->width ) <<
                "read row " << rows;

        OOE_CHECK( rows == reader->height ) << "reader read complete image";
    }

    erase( path );
}

OOE_TEST( 4 )
{
    std::cerr << "jpeg2000";
    const c8 path[] = _PATH_TMP "ooe.test.jp2";

    image input[] =
    {
        make_image< u8 >( 255, 0, 0, image_format::rgb_u8 ),
        make_image< u8 >( 0, 255, 0, image_format::rgb_u8 ),
        make_image< u8 >( 0, 0, 255, image_format::rgb_u8 )
    };

    for ( u8 i = 0; i != 3; ++i )
    {
        jpeg2000::encode( input[ i ], descriptor( path, descriptor::write_new ) );
        image output = jpeg2000::decode( descriptor( path ) );

        OOE_CHECK( byte_size( input[ i ] ) == byte_size( output ) ) << "decoded sizes do not match";
        OOE_CHECK( !std::memcmp( input[ i ].get(), output.get(), byte_size( output ) ) ) <<
            "decoded data does not match";
    }

    erase( path );
}

OOE_TEST( 5 )
{
    std::cerr << "png";
    const c8 path[] = _PATH_TMP "ooe.test.png";

    image input[] =
    {
        make_image< u8 >( 255, 0, 0, image_format::rgba_u8 ),
        make_image< u8 >( 0, 255, 0, image_format::rgba_u8 ),
        make_image< u8 >( 0, 0, 255, image_format::rgba_u8 )
    };

    for ( u8 i = 0; i != 3; ++i )
    {
        png::encode( input[ i ], descriptor( path, descriptor::write_new ) );
        image output = png::decode( descriptor( path ) );
        OOE_CHECK( input[ i ] == output ) << "decoded metadata does not match";
        OOE_CHECK( byte_size( input[ i ] ) == byte_size( output ) ) << "decoded sizes do not match";
        OOE_CHECK( !std::memcmp( input[ i ].get(), output.get(), byte_size( output ) ) ) <<
            "decoded data does not match";

        reader_ptr reader = png::open( descriptor( path ) );
        OOE_CHECK( input[ i ] == *reader ) << "reader metadata does not match";
        OOE_CHECK( byte_size( input[ i ] ) == byte_size( *reader ) ) <<
            "reader size does not match";

        std::memset( output.get(), 0, byte_size( output ) );
        up_t row_size = ooe::row_size( *reader );
        u32 rows = 0;

        for ( u8* j = output.as< u8 >(); reader->decode_row(); ++rows, j += row_size )
            OOE_CHECK( reader->read_pixels( j, reader->width ) == reader->width ) <<
                "read row " << rows;

        OOE_CHECK( rows == reader->height ) << "reader did not read complete image";
        OOE_CHECK( !std::memcmp( input[ i ].get(), output.get(), byte_size( output ) ) ) <<
            "reader data does not match";
    }

    erase( path );
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
