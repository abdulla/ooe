/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_PLATFORM_DARWIN_FOUNDATION_VISUAL_EVENT_QUEUE_FORWARD_HPP
#define OOE_PLATFORM_DARWIN_FOUNDATION_VISUAL_EVENT_QUEUE_FORWARD_HPP

#include "foundation/utility/macro.hpp"
#include "foundation/utility/namespace.hpp"

#ifdef __OBJC__
	@class NSAutoreleasePool;
#else
	struct NSAutoreleasePool;
#endif

OOE_NAMESPACE_BEGIN( ( ooe ) )

enum key_type
{
	key_escape		= 0x001b,
	key_return		= 0x000d,
	key_backspace	= 0x007f,

	key_up			= 0xf700,
	key_down		= 0xf701,
	key_left		= 0xf702,
	key_right		= 0xf703
};

OOE_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

class OOE_VISIBLE event_queue
{
protected:
	mutable bool delta;

	event_queue( void );
};

OOE_NAMESPACE_END( ( ooe )( platform ) )

#endif	// OOE_PLATFORM_DARWIN_FOUNDATION_VISUAL_EVENT_QUEUE_FORWARD_HPP
