/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_PLATFORM_POSIX_FOUNDATION_VISUAL_EVENT_QUEUE_FORWARD_HPP
#define OOE_PLATFORM_POSIX_FOUNDATION_VISUAL_EVENT_QUEUE_FORWARD_HPP

#include "foundation/utility/function.hpp"
#include "foundation/utility/fundamental.hpp"

#ifndef OOE_X_HEADER_INCLUDED
typedef struct _XDisplay Display;
#endif	// OOE_X_HEADER_INCLUDED

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

struct event_queue
{
	Display* display;
	mutable up_t wm_delete;
	mutable s32 x;
	mutable s32 y;
	mutable function< void ( void ) > warp;

	event_queue( void );
	~event_queue( void );
};

OOE_NAMESPACE_END( ( ooe )( platform ) )

#endif	// OOE_PLATFORM_POSIX_FOUNDATION_VISUAL_EVENT_QUEUE_FORWARD_HPP
