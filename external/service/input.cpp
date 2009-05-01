/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "external/service/input.hpp"

namespace ooe
{
//--- input_provider -----------------------------------------------------------
	input_provider::input_provider( const event_queue& queue_ )
		: motion(), button(), key(), queue( queue_ )
	{
	}

//--- input_service ------------------------------------------------------------
	void input_service( void* pointer )
	{
		input_provider& provider = *static_cast< input_provider* >( pointer );

		event::type type;
		event event;

		while ( ( type = provider.queue.next_event( event, false ) ) )
		{
			switch ( type )
			{
			case event::motion_flag:
				provider.motion( event.motion.x, event.motion.y );
				break;

			case event::button_flag:
				provider.button( event.button.value, event.button.press );
				break;

			case event::key_flag:
				provider.key( event.key.value, event.key.press );
				break;

			default:
				break;
			}
		}
	}

//--- input_register -----------------------------------------------------------
	void input_register( service& service, input_provider& provider )
	{
		service.insert( input_service, &provider );
		service.insert( "motion", provider.motion );
		service.insert( "button", provider.button );
		service.insert( "key", provider.key );
	}
}
