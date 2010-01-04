/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

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
			: image( 16, 16, uncompressed_image::rgb_u8 )
		{
		}
	};

	typedef unit::group< empty_t, data, 3 > group_type;
	typedef group_type::fixture_type fixture_type;
	group_type group( "image" );
}

namespace ooe
{
	namespace unit
	{
		template<>
		template<>
			void fixture_type::test< 0 >( empty_t& )
		{
			std::cerr << "uncompressed_image\n";

			check( "image.width == 16", image.width == 16 );
			check( "image.height == 16", image.height == 16 );
			check( "image.format == uncompressed_image::rgb_u8",
				image.format == uncompressed_image::rgb_u8 );
			check( "image.get()", image.get() );
			check( "image.channels() == 3", image.channels() == 3 );
			check( "image.channel_size() == 1", image.channel_size() == 1 );
			check( "image.pixel_size() == 3", image.pixel_size() == 3 );
			check( "image.row_size() == 48", image.row_size() == 48 );
			check( "image.byte_size() == 768", image.byte_size() == 768 );
		}

		template<>
		template<>
			void fixture_type::test< 1 >( empty_t& )
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
			void fixture_type::test< 2 >( empty_t& )
		{
			std::cerr << "dds\n";

			compressed_image dxt1 = dxt::encode( image, compressed_image::rgba_dxt1 );
			dds::encode( dxt1, descriptor( "/dev/null", descriptor::write_new ) );
		}
	}
}
