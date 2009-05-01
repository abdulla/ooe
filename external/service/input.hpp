/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_EXTERNAL_SERVICE_INPUT_HPP
#define OOE_EXTERNAL_SERVICE_INPUT_HPP

#include "foundation/general/event_queue.hpp"
#include "foundation/utility/signal.hpp"

#include "external/service/service.hpp"

namespace ooe
{
	typedef signal< void ( s32, s32 ) > motion_signal;
	typedef signal< void ( u32, bool ) > button_signal;
	typedef signal< void ( u32, bool ) > key_signal;

	typedef slot< void ( s32, s32 ) > motion_slot;
	typedef slot< void ( u32, bool ) > button_slot;
	typedef slot< void ( u32, bool ) > key_slot;

	struct input_provider
	{
		motion_signal motion;
		button_signal button;
		key_signal key;

		const event_queue& queue;

		input_provider( const event_queue& );
	};

	void input_service( void* );
	void input_register( service&, input_provider& );
}

#endif	// OOE_EXTERNAL_SERVICE_INPUT_HPP
