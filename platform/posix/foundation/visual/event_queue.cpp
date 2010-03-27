/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <cerrno>

#include <poll.h>
#include <X11/Xutil.h>

#include "foundation/utility/error.hpp"

#define OOE_X_HEADER_INCLUDED
#include "foundation/visual/event_queue.hpp"
#undef OOE_X_HEADER_INCLUDED

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

//--- event_queue ----------------------------------------------------------------------------------
event_queue::event_queue( void )
	: display( XOpenDisplay( 0 ) ), wm_delete(), x(), y(), warp()
{
	if ( !display )
		throw error::runtime( "view: " ) << "Display not opened";

	wm_delete = XInternAtom( display, "WM_DELETE_WINDOW", false );
}

event_queue::~event_queue( void )
{
	XCloseDisplay( display );
}

OOE_NAMESPACE_END( ( ooe )( platform ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- event_queue ----------------------------------------------------------------------------------
event::type event_queue::next_event( event& event, epoch_t timeout ) const
{
	pollfd array = { ConnectionNumber( display ), POLLIN, 0 };
	timespec interval = { timeout._0, timeout._1 };

	if ( !XPending( display ) )
	{
		s32 status = ppoll( &array, 1, &interval, 0 );

		if ( !status )
			return event::none;
		else if ( status == -1 )
			throw error::runtime( "event_queue: " ) <<
				"Unable to poll display connection: " << error::number( errno );
	}

	XEvent xevent;
	XNextEvent( display, &xevent );

	switch ( xevent.type )
	{
	case MotionNotify:
		event.motion.x = xevent.xmotion.x - x;
		event.motion.y = xevent.xmotion.y - y;

		if ( !event.motion.x && !event.motion.y )
			return event::ignore;

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
			return event::exit;
		else
			return event::ignore;

	default:
		return event::ignore;
	}
}

OOE_NAMESPACE_END( ( ooe ) )
