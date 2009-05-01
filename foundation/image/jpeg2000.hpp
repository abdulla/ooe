/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IMAGE_JPEG2000_HPP
#define OOE_FOUNDATION_IMAGE_JPEG2000_HPP

#include "foundation/image/image.hpp"
#include "foundation/io/descriptor.hpp"

namespace ooe
{
	namespace jpeg2000
	{
		uncompressed_image decode( const descriptor& ) OOE_VISIBLE;
		uncompressed_image decode( const void*, up_t ) OOE_VISIBLE;

		void encode( const uncompressed_image&, const descriptor& desc ) OOE_VISIBLE;
	}
}

#endif	// OOE_FOUNDATION_IMAGE_JPEG2000_HPP
