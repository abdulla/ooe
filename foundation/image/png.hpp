/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IMAGE_PNG_HPP
#define OOE_FOUNDATION_IMAGE_PNG_HPP

#include "foundation/image/image.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

class descriptor;

OOE_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( png ) )

reader_ptr open( const descriptor& ) OOE_VISIBLE;
image decode( const descriptor& ) OOE_VISIBLE;
void encode( const image&, const descriptor& ) OOE_VISIBLE;

OOE_NAMESPACE_END( ( ooe )( png ) )

#endif  // OOE_FOUNDATION_IMAGE_PNG_HPP
