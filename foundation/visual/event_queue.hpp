/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_VISUAL_EVENT_QUEUE_HPP
#define OOE_FOUNDATION_VISUAL_EVENT_QUEUE_HPP

#include "foundation/executable/timer.hpp"
#include "foundation/visual/event_queue_forward.hpp"

namespace ooe
{
//--- event --------------------------------------------------------------------
	struct event
	{
		enum type
		{
			none,
			exit,
			ignore,

			motion_flag,
			button_flag,
			key_flag
		};

		union
		{
			struct
			{
				s32 x;
				s32 y;
			} motion;

			struct
			{
				u32 value;
				bool press;
			} button, key;
		};
	};

//--- event_queue --------------------------------------------------------------
	struct OOE_VISIBLE event_queue
		: private noncopyable, public platform::event_queue
	{
		event::type next_event( event&, epoch_t = epoch_t( 0, 0 ) ) const;
	};
}

#endif	// OOE_FOUNDATION_VISUAL_EVENT_QUEUE_HPP
