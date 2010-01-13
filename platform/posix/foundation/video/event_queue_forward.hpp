/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_VIDEO_EVENT_QUEUE_FORWARD_HPP
#define OOE_FOUNDATION_VIDEO_EVENT_QUEUE_FORWARD_HPP

#include "foundation/utility/function.hpp"
#include "foundation/utility/fundamental.hpp"
#include "foundation/utility/macro.hpp"

#ifndef OOE_X_HEADER_INCLUDED
typedef struct _XDisplay Display;
#endif	// OOE_X_HEADER_INCLUDED

namespace ooe
{
	namespace platform
	{
		class event_queue;
	}

	class OOE_VISIBLE platform::event_queue
	{
	public:
		Display* display;
		mutable up_t wm_delete;
		mutable s32 x;
		mutable s32 y;
		mutable function< void ( void ) > grab;
		mutable function< void ( void ) > warp;

		event_queue( void );
		~event_queue( void );
	};
}

#endif	// OOE_FOUNDATION_VIDEO_EVENT_QUEUE_FORWARD_HPP
