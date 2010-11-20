/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IMAGE_JPEG2000_HPP
#define OOE_FOUNDATION_IMAGE_JPEG2000_HPP

#include "foundation/image/image.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

class descriptor;

OOE_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( jpeg2000 ) )

image decode( const descriptor& ) OOE_VISIBLE;
void encode( const image&, const descriptor& desc ) OOE_VISIBLE;

OOE_NAMESPACE_END( ( ooe )( jpeg2000 ) )

#endif  // OOE_FOUNDATION_IMAGE_JPEG2000_HPP
