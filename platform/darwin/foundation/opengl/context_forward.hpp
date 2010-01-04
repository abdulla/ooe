/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_CONTEXT_FORWARD_HPP
#define OOE_FOUNDATION_OPENGL_CONTEXT_FORWARD_HPP

#ifdef __OBJC__
	@class NSOpenGLContext;
#else
	struct NSOpenGLContext;
#endif

namespace ooe
{
	namespace platform
	{
		typedef NSOpenGLContext* context_type;
	}
}

#endif	// OOE_FOUNDATION_OPENGL_CONTEXT_FORWARD_HPP
