/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IMAGE_EXR_HPP
#define OOE_FOUNDATION_IMAGE_EXR_HPP

#include "foundation/image/image.hpp"
#include "foundation/io/descriptor.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( exr ) )

uncompressed_image decode( const descriptor& ) OOE_VISIBLE;
uncompressed_image decode( const void*, up_t ) OOE_VISIBLE;

OOE_NAMESPACE_END( ( ooe )( exr ) )

#endif  // OOE_FOUNDATION_IMAGE_EXR_HPP
