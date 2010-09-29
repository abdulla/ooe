/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

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
    : display( XOpenDisplay( 0 ) ), wm_delete()
{
    if ( !display )
        throw error::runtime( "view: " ) << "Unable to open display connection";

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
        event.motion.x = xevent.xmotion.x;
        event.motion.y = xevent.xmotion.y;
        return event::motion_flag;

    case KeyPress:
    case KeyRelease:
        event.key.value = XLookupKeysym( &xevent.xkey, 0 );
        event.key.press = xevent.type == KeyPress;
        return event::key_flag;

    case ButtonPress:
    case ButtonRelease:
        event.button.value = xevent.xbutton.button;
        event.button.press = xevent.type == ButtonPress;
        return event::button_flag;

    case ClientMessage:
        return xevent.xclient.data.l[ 0 ] == static_cast< sp_t >( wm_delete ) ?
            event::exit : event::ignore;

    default:
        return event::ignore;
    }
}

OOE_NAMESPACE_END( ( ooe ) )
