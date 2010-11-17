/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IMAGE_PNG_HPP
#define OOE_FOUNDATION_IMAGE_PNG_HPP

#include "foundation/image/image.hpp"
#include "foundation/io/descriptor.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( png ) )

uncompressed_image decode( const descriptor& ) OOE_VISIBLE;
uncompressed_image decode( const void*, up_t ) OOE_VISIBLE;

void encode( const uncompressed_image&, const descriptor& ) OOE_VISIBLE;

OOE_NAMESPACE_END( ( ooe )( png ) )

#endif  // OOE_FOUNDATION_IMAGE_PNG_HPP
