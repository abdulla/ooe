/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_GENERAL_EVENT_QUEUE_FORWARD_HPP
#define OOE_FOUNDATION_GENERAL_EVENT_QUEUE_FORWARD_HPP

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
		mutable function< void ( void ) > configure;

		event_queue( void );
		~event_queue( void );

	protected:
		mutable s32 x;
		mutable s32 y;
	};
}

#endif	// OOE_FOUNDATION_GENERAL_EVENT_QUEUE_FORWARD_HPP
