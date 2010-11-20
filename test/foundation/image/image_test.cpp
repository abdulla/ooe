/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cstring>

#include <paths.h>

#include "foundation/image/dds.hpp"
#include "foundation/image/jpeg.hpp"
#include "foundation/image/jpeg2000.hpp"
#include "foundation/image/png.hpp"
#include "foundation/io/descriptor.hpp"
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

template<>
template<>
    void fixture_type::test< 0 >( anonymous_t& )
{
    std::cerr << "uncompressed_image\n";
    ooe::image image( 16, 16, image_format::rgba_u8 );

    OOE_CHECK( "image.width == 16", image.width == 16 );
    OOE_CHECK( "image.height == 16", image.height == 16 );
    OOE_CHECK( "image.format == uncompressed_image::rgba_u8",
        image.format == image_format::rgba_u8 );
    OOE_CHECK( "image.get()", image.get() );
    OOE_CHECK( "subpixels( image ) == 4", subpixels( image ) == 4 );
    OOE_CHECK( "subpixel_size( image ) == 1", subpixel_size( image ) == 1 );
    OOE_CHECK( "pixel_size( image ) == 4", pixels( image ) == 256 );
    OOE_CHECK( "pixel_size( image ) == 4", pixel_size( image ) == 4 );
    OOE_CHECK( "row_size( image ) == 64", row_size( image ) == 64 );
    OOE_CHECK( "byte_size( image ) == 1024", byte_size( image ) == 1024 );
}

template<>
template<>
    void fixture_type::test< 1 >( anonymous_t& )
{
    std::cerr << "dxt\n";
    ooe::image image = make_image< u8 >( 255, 255, 255, image_format::rgba_u8 );

    ooe::image dxt1 = dxt::encode( image, image_format::rgba_dxt1 );
    ooe::image dxt1_uncompressed = dxt::decode( dxt1 );
    OOE_CHECK( "block_size( dxt1 ) == 8", block_size( dxt1 ) == 8 );
    OOE_CHECK( "byte_size( dxt1 ) == 128", byte_size( dxt1 ) == 128 );

    ooe::image dxt3 = dxt::encode( image, image_format::rgba_dxt3 );
    ooe::image dxt3_uncompressed = dxt::decode( dxt3 );
    OOE_CHECK( "block_size( dxt3 ) == 16", block_size( dxt3 ) == 16 );
    OOE_CHECK( "byte_size( dxt3 ) == 256", byte_size( dxt3 ) == 256 );

    ooe::image dxt5 = dxt::encode( image, image_format::rgba_dxt5 );
    ooe::image dxt5_uncompressed = dxt::decode( dxt5 );
    OOE_CHECK( "block_size( dxt5 ) == 16", block_size( dxt5 ) == 16 );
    OOE_CHECK( "byte_size( dxt5 ) == 256", byte_size( dxt5 ) == 256 );
}

template<>
template<>
    void fixture_type::test< 2 >( anonymous_t& )
{
    std::cerr << "dds\n";
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

            OOE_CHECK( "dxt sizes match", byte_size( dxt_in ) == byte_size( dxt_out ) );
            OOE_CHECK( "dxt data matches",
                !std::memcmp( dxt_in.get(), dxt_out.get(), byte_size( dxt_in ) ) );

            OOE_CHECK( "decoded sizes match", byte_size( input[ j ] ) == byte_size( output ) );
            OOE_CHECK( "decoded data matches",
                !std::memcmp( input[ j ].get(), output.get(), byte_size( output ) ) );
        }
    }

    unlink( path );
}

template<>
template<>
    void fixture_type::test< 3 >( anonymous_t& )
{
    std::cerr << "jpeg\n";
    const c8 path[] = _PATH_TMP "ooe.test.jpg";

    image input[] =
    {
        make_image< u8 >( 255, 0, 0, image_format::rgb_u8 ),
        make_image< u8 >( 0, 255, 0, image_format::rgb_u8 ),
        make_image< u8 >( 0, 0, 255, image_format::rgb_u8 )
    };

    for ( u8 i = 0; i != 3; ++i )
    {
        jpeg::encode( input[ i ], descriptor( path, descriptor::write_new ) );
        image output = jpeg::decode( descriptor( path ) );

        OOE_CHECK( "decoded sizes match", byte_size( input[ i ] ) == byte_size( output ) );
        // no test for data match due to minor differences caused by lossy compression
    }

    unlink( path );
}

template<>
template<>
    void fixture_type::test< 4 >( anonymous_t& )
{
    std::cerr << "jpeg2000\n";
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

        OOE_CHECK( "decoded sizes match", byte_size( input[ i ] ) == byte_size( output ) );
        OOE_CHECK( "decoded data matches",
            !std::memcmp( input[ i ].get(), output.get(), byte_size( output ) ) );
    }

    unlink( path );
}

template<>
template<>
    void fixture_type::test< 5 >( anonymous_t& )
{
    std::cerr << "png\n";
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

        OOE_CHECK( "decoded sizes match", byte_size( input[ i ] ) == byte_size( output ) );
        OOE_CHECK( "decoded data matches",
            !std::memcmp( input[ i ].get(), output.get(), byte_size( output ) ) );

        reader_ptr reader = png::open( descriptor( path ) );
        OOE_CHECK( "reader size matches", byte_size( input[ i ] ) == byte_size( *reader ) );

        std::memset( output.get(), 0, byte_size( output ) );
        up_t row_size = ooe::row_size( *reader );
        u32 rows = 0;

        for ( u8* j = output.as< u8 >(); reader->decode_row(); ++rows, j += row_size )
            OOE_CHECK( "read row " << rows,
                reader->read_pixels( j, reader->width ) == reader->width );

        OOE_CHECK( "reader read complete image", rows == reader->height );
        OOE_CHECK( "reader data matches",
            !std::memcmp( input[ i ].get(), output.get(), byte_size( output ) ) );
    }

    unlink( path );
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
