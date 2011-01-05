/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_TARGET_HPP
#define OOE_FOUNDATION_OPENGL_TARGET_HPP

#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

struct target
    : public ooe::target
{
    u32 id;
    const u32 width;
    const u32 height;

    target( const image_metadata& );
    virtual ~target( void );
};

OOE_NAMESPACE_END( ( ooe )( opengl ) )

#endif  // OOE_FOUNDATION_OPENGL_TARGET_HPP
