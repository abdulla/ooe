/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_PLATFORM_POSIX_FOUNDATION_VISUAL_EVENT_QUEUE_FORWARD_HPP
#define OOE_PLATFORM_POSIX_FOUNDATION_VISUAL_EVENT_QUEUE_FORWARD_HPP

#include "foundation/utility/fundamental.hpp"

#ifndef OOE_X_HEADER_INCLUDED
typedef struct _XDisplay Display;
#endif	// OOE_X_HEADER_INCLUDED

OOE_NAMESPACE_BEGIN( ( ooe ) )

enum key_type
{
	key_escape			= 0xff1b,
	key_return			= 0xff0d,
	key_backspace		= 0xff08,
	key_shift_left		= 0xffe1,
	key_shift_right		= 0xffe2,
	key_command_left	= 0xffeb,
	key_command_right	= 0xffec,

	key_up				= 0xff52,
	key_down			= 0xff54,
	key_left			= 0xff51,
	key_right			= 0xff53
};

OOE_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

struct OOE_VISIBLE event_queue
{
	Display* display;
	mutable up_t wm_delete;

	event_queue( void );
	~event_queue( void );
};

OOE_NAMESPACE_END( ( ooe )( platform ) )

#endif	// OOE_PLATFORM_POSIX_FOUNDATION_VISUAL_EVENT_QUEUE_FORWARD_HPP
