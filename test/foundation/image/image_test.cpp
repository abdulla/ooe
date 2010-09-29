/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/image/dds.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

namespace
{
    using namespace ooe;

    struct data
    {
    protected:
        uncompressed_image image;

        data( void )
            : image( 16, 16, uncompressed_image::rgba_u8 )
        {
        }
    };

    typedef unit::group< anonymous_t, data, 3 > group_type;
    typedef group_type::fixture_type fixture_type;
    group_type group( "image" );
}

namespace ooe
{
    namespace unit
    {
        template<>
        template<>
            void fixture_type::test< 0 >( anonymous_t& )
        {
            std::cerr << "uncompressed_image\n";

            OOE_CHECK( "image.width == 16", image.width == 16 );
            OOE_CHECK( "image.height == 16", image.height == 16 );
            OOE_CHECK( "image.format == uncompressed_image::rgba_u8",
                image.format == uncompressed_image::rgba_u8 );
            OOE_CHECK( "image.get()", image.get() );
            OOE_CHECK( "image.channels() == 4", image.channels() == 4 );
            OOE_CHECK( "image.channel_size() == 1", image.channel_size() == 1 );
            OOE_CHECK( "image.pixel_size() == 4", image.pixel_size() == 4 );
            OOE_CHECK( "image.row_size() == 64", image.row_size() == 64 );
            OOE_CHECK( "image.byte_size() == 1024", image.byte_size() == 1024 );
        }

        template<>
        template<>
            void fixture_type::test< 1 >( anonymous_t& )
        {
            std::cerr << "dxt\n";

            compressed_image dxt1 = dxt::encode( image, compressed_image::rgba_dxt1 );
            uncompressed_image dxt1_uncompressed = dxt::decode( dxt1 );
            compressed_image dxt3 = dxt::encode( image, compressed_image::rgba_dxt3 );
            uncompressed_image dxt3_uncompressed = dxt::decode( dxt3 );
            compressed_image dxt5 = dxt::encode( image, compressed_image::rgba_dxt5 );
            uncompressed_image dxt5_uncompressed = dxt::decode( dxt5 );
        }

        template<>
        template<>
            void fixture_type::test< 2 >( anonymous_t& )
        {
            std::cerr << "dds\n";

            compressed_image dxt1 = dxt::encode( image, compressed_image::rgba_dxt1 );
            dds::encode( dxt1, descriptor( "/dev/null", descriptor::write_new ) );
        }
    }
}
