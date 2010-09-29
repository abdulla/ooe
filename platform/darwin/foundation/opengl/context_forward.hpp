/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_PLATFORM_DARWIN_FOUNDATION_OPENGL_CONTEXT_FORWARD_HPP
#define OOE_PLATFORM_DARWIN_FOUNDATION_OPENGL_CONTEXT_FORWARD_HPP

#include "foundation/utility/namespace.hpp"

#ifdef __OBJC__
    @class NSOpenGLContext;
#else
    struct NSOpenGLContext;
#endif

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

typedef NSOpenGLContext* context_type;

OOE_NAMESPACE_END( ( ooe )( platform ) )

#endif  // OOE_PLATFORM_DARWIN_FOUNDATION_OPENGL_CONTEXT_FORWARD_HPP
