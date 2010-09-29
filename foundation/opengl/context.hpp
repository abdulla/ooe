/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_CONTEXT_HPP
#define OOE_FOUNDATION_OPENGL_CONTEXT_HPP

#include "foundation/opengl/context_forward.hpp"
#include "foundation/utility/macro.hpp"

namespace ooe
{
    struct view_data;

    platform::context_type context_construct( const view_data& ) OOE_VISIBLE;
    void context_destruct( const view_data&, platform::context_type ) OOE_VISIBLE;
    void context_current( const view_data&, platform::context_type ) OOE_VISIBLE;
    void context_sync( const view_data&, platform::context_type, bool ) OOE_VISIBLE;
    void context_swap( const view_data&, platform::context_type ) OOE_VISIBLE;
}

#endif  // OOE_FOUNDATION_OPENGL_CONTEXT_HPP
