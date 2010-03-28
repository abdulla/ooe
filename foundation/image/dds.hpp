/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IMAGE_DDS_HPP
#define OOE_FOUNDATION_IMAGE_DDS_HPP

#include "foundation/image/image.hpp"
#include "foundation/io/descriptor.hpp"

namespace ooe
{
	namespace dds
	{
		compressed_image decode( const descriptor& ) OOE_VISIBLE;
		compressed_image decode( const void*, up_t ) OOE_VISIBLE;

		void encode( const compressed_image&, const descriptor& ) OOE_VISIBLE;
	}

	namespace dxt
	{
		uncompressed_image decode( const compressed_image& ) OOE_VISIBLE;
		compressed_image encode( const uncompressed_image&, image::type ) OOE_VISIBLE;
	}
}

#endif	// OOE_FOUNDATION_IMAGE_DDS_HPP
