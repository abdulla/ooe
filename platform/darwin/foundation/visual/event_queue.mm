/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <AppKit/AppKit.h>

#include "foundation/utility/error.hpp"
#include "foundation/visual/event_queue.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

//--- event_queue ----------------------------------------------------------------------------------
event_queue::event_queue( void )
	: delta( false )
{
	if ( ![ NSApplication sharedApplication ] )
		throw error::runtime( "executable: " ) << "Unable to initialise shared application";

	[ NSApp finishLaunching ];
}

OOE_NAMESPACE_END( ( ooe )( platform ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- event_queue ----------------------------------------------------------------------------------
event::type event_queue::next_event( event& event, epoch_t timeout ) const
{
	NSDate* date = [ NSDate dateWithTimeIntervalSinceNow: timeout._0 + timeout._1 * 1e-9 ];
	NSEvent* nsevent = [ NSApp nextEventMatchingMask: NSAnyEventMask untilDate: date
		inMode: NSDefaultRunLoopMode dequeue: true ];

	if ( !nsevent )
		return event::none;

	switch ( nsevent.type )
	{
	case NSMouseMoved:
	case NSLeftMouseDragged:
	case NSRightMouseDragged:
	case NSOtherMouseDragged:
		CGGetLastMouseDelta( &event.motion.x, &event.motion.y );

		if ( !delta )
		{
			delta = true;
			return event::ignore;
		}

		return event::motion_flag;

	case NSKeyDown:
		event.key.value = [ nsevent.charactersIgnoringModifiers characterAtIndex: 0 ];
		event.key.press = true;
		return event::key_flag;

	case NSKeyUp:
		event.key.value = [ nsevent.charactersIgnoringModifiers characterAtIndex: 0 ];
		event.key.press = false;
		return event::key_flag;

	case NSLeftMouseDown:
		event.button.value = 0;
		event.button.press = true;
		return event::button_flag;

	case NSRightMouseDown:
		event.button.value = 1;
		event.button.press = true;
		return event::button_flag;

	case NSOtherMouseDown:
		event.button.value = 2;
		event.button.press = true;
		return event::button_flag;

	case NSLeftMouseUp:
		event.button.value = 0;
		event.button.press = false;
		return event::button_flag;

	case NSRightMouseUp:
		event.button.value = 1;
		event.button.press = false;
		return event::button_flag;

	case NSOtherMouseUp:
		event.button.value = 2;
		event.button.press = false;
		return event::button_flag;

	default:
		[ NSApp sendEvent: nsevent ];
		return event::ignore;
	}
}

OOE_NAMESPACE_END( ( ooe ) )
