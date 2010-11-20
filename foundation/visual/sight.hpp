/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_VISUAL_SIGHT_HPP
#define OOE_FOUNDATION_VISUAL_SIGHT_HPP

#include "foundation/image/image.hpp"
#include "foundation/utility/noncopyable.hpp"
#include "foundation/visual/sight_forward.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

struct OOE_VISIBLE sight
    : private noncopyable, private platform::sight
{
    using platform::sight::call_type;

    sight( const call_type&, u16, u16 );

    static image_format::type format( void );
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_VISUAL_SIGHT_HPP
