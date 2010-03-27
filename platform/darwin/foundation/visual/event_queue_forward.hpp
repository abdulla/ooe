/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_PLATFORM_DARWIN_FOUNDATION_VISUAL_EVENT_QUEUE_FORWARD_HPP
#define OOE_PLATFORM_DARWIN_FOUNDATION_VISUAL_EVENT_QUEUE_FORWARD_HPP

#include "foundation/utility/namespace.hpp"

#ifdef __OBJC__
	@class NSAutoreleasePool;
#else
	struct NSAutoreleasePool;
#endif

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

class event_queue
{
protected:
	mutable bool delta;

	event_queue( void );
};

OOE_NAMESPACE_END( ( ooe )( platform ) )

#endif	// OOE_PLATFORM_DARWIN_FOUNDATION_VISUAL_EVENT_QUEUE_FORWARD_HPP
