/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IMAGE_DDS_HPP
#define OOE_FOUNDATION_IMAGE_DDS_HPP

#include "foundation/image/image.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

class descriptor;

OOE_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( dds ) )

image decode( const descriptor& ) OOE_VISIBLE;
void encode( const image&, const descriptor& ) OOE_VISIBLE;

OOE_NAMESPACE_END( ( ooe )( dds ) )

OOE_NAMESPACE_BEGIN( ( ooe )( dxt ) )

image decode( const image& ) OOE_VISIBLE;
image encode( const image&, image_format::type ) OOE_VISIBLE;

OOE_NAMESPACE_END( ( ooe )( dxt ) )

#endif  // OOE_FOUNDATION_IMAGE_DDS_HPP
