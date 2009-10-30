/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <X11/Xutil.h>

#include "foundation/utility/error.hpp"

#define OOE_X_HEADER_INCLUDED
#include "foundation/general/event_queue.hpp"
#undef OOE_X_HEADER_INCLUDED

namespace ooe
{
//--- platform::event_queue ----------------------------------------------------
	platform::event_queue::event_queue( void )
		: display( XOpenDisplay( 0 ) ), wm_delete(), x(), y(), grab(), warp()
	{
		if ( !display )
			throw error::runtime( "view: " ) << "Display not opened";

		wm_delete = XInternAtom( display, "WM_DELETE_WINDOW", 1 );
	}

	platform::event_queue::~event_queue( void )
	{
		XCloseDisplay( display );
	}

//--- event_queue --------------------------------------------------------------
	event::type event_queue::next_event( event& event, bool wait ) const
	{
		while ( true )
		{
			if ( !wait && !XPending( display ) )	// do not wait for an event to occur
				return event::none;

			XEvent xevent;
			XNextEvent( display, &xevent );

			switch ( xevent.type )
			{
			case MotionNotify:
				event.motion.x = xevent.xmotion.x - x;
				event.motion.y = xevent.xmotion.y - y;
				warp();
				return event::motion_flag;

			case KeyRelease:
				event.key.value = XLookupKeysym( &xevent.xkey, 0 );
				event.key.press = false;
				return event::key_flag;

			case KeyPress:
				event.key.value = XLookupKeysym( &xevent.xkey, 0 );
				event.key.press = true;
				return event::key_flag;

			case ButtonRelease:
				event.button.value = xevent.xbutton.button;
				event.button.press = false;
				return event::button_flag;

			case ButtonPress:
				event.button.value = xevent.xbutton.button;
				event.button.press = true;
				return event::button_flag;

			case ClientMessage:
				if ( xevent.xclient.data.l[ 0 ] == static_cast< sp_t >( wm_delete ) )
					return event::exit_flag;
				else
					break;

			case ConfigureNotify:
				grab();
				break;

			default:
				break;
			}
		}
	}
}
