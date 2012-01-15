/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_DARWIN_CONTEXT_FORWARD_HPP
#define OOE_FOUNDATION_OPENGL_DARWIN_CONTEXT_FORWARD_HPP

#include "foundation/utility/namespace.hpp"

#ifdef __OBJC__
    @class NSOpenGLContext;
#else
    struct NSOpenGLContext;
#endif

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

typedef NSOpenGLContext* context_type;

OOE_NAMESPACE_END( ( ooe )( platform ) )

#endif  // OOE_FOUNDATION_OPENGL_DARWIN_CONTEXT_FORWARD_HPP
