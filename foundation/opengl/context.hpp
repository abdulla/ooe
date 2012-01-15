/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_CONTEXT_HPP
#define OOE_FOUNDATION_OPENGL_CONTEXT_HPP

#include "foundation/utility/macro.hpp"

#include OOE_PATH( foundation/opengl, context_forward.hpp )

OOE_NAMESPACE_BEGIN( ( ooe ) )

class view_data;

platform::context_type context_construct( const view_data& ) OOE_VISIBLE;
void context_destruct( const view_data&, platform::context_type ) OOE_VISIBLE;
void context_current( const view_data&, platform::context_type ) OOE_VISIBLE;
void context_sync( const view_data&, platform::context_type, bool ) OOE_VISIBLE;
void context_swap( const view_data&, platform::context_type ) OOE_VISIBLE;

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_OPENGL_CONTEXT_HPP
