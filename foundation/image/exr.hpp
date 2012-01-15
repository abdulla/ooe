/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IMAGE_EXR_HPP
#define OOE_FOUNDATION_IMAGE_EXR_HPP

#include "foundation/image/image.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

class descriptor;

OOE_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( exr ) )

image decode( const descriptor& ) OOE_VISIBLE;

OOE_NAMESPACE_END( ( ooe )( exr ) )

#endif  // OOE_FOUNDATION_IMAGE_EXR_HPP
