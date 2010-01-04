/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_GENERAL_EVENT_QUEUE_FORWARD_HPP
#define OOE_FOUNDATION_GENERAL_EVENT_QUEUE_FORWARD_HPP

#include "foundation/utility/macro.hpp"

#ifdef __OBJC__
	@class NSAutoreleasePool;
#else
	struct NSAutoreleasePool;
#endif

namespace ooe
{
	namespace platform
	{
		class event_queue;
	}

	class OOE_VISIBLE platform::event_queue
	{
	protected:
		mutable bool delta;

		event_queue( void );
	};
}

#endif	// OOE_FOUNDATION_GENERAL_EVENT_QUEUE_FORWARD_HPP
