/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <AppKit/AppKit.h>

#include "foundation/utility/error.hpp"
#include "foundation/visual/event_queue.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

enum
{
    raw_shift_left      = 56,
    raw_shift_right     = 60,
    raw_command_left    = 55,
    raw_command_right   = 54
};

typedef tuple< u32, bool > modifier_tuple;

modifier_tuple modifier( NSEvent* nsevent )
{
    switch ( nsevent.keyCode )
    {
    case raw_shift_left:
        return modifier_tuple( key_shift_left, nsevent.modifierFlags & NSShiftKeyMask );

    case raw_shift_right:
        return modifier_tuple( key_shift_right, nsevent.modifierFlags & NSShiftKeyMask );

    case raw_command_left:
        return modifier_tuple( key_command_left, nsevent.modifierFlags & NSCommandKeyMask );

    case raw_command_right:
        return modifier_tuple( key_command_right, nsevent.modifierFlags & NSCommandKeyMask );

    default:
        return modifier_tuple( 0, false );
    }
}

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

//--- event_queue ----------------------------------------------------------------------------------
event_queue::event_queue( void )
{
    if ( ![ NSApplication sharedApplication ] )
        throw error::runtime( "executable: " ) << "Unable to initialise shared application";

    [ NSApp finishLaunching ];
}

OOE_NAMESPACE_END( ( ooe )( platform ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- event_queue ----------------------------------------------------------------------------------
event::type event_queue::dequeue( event& event, epoch_t timeout ) const
{
    NSDate* date = [ NSDate dateWithTimeIntervalSinceNow: timeout._0 + timeout._1 * 1e-9 ];
    NSEvent* nsevent = [ NSApp nextEventMatchingMask: NSAnyEventMask untilDate: date
        inMode: NSDefaultRunLoopMode dequeue: true ];

    if ( !nsevent )
        return event::none;

    NSView* view;
    modifier_tuple tuple;

    switch ( nsevent.type )
    {
    case NSMouseMoved:
    case NSLeftMouseDragged:
    case NSRightMouseDragged:
    case NSOtherMouseDragged:
        view = nsevent.window.contentView;

        if ( ![ view mouse: nsevent.locationInWindow inRect: view.bounds ] )
            return event::ignore;

        event.motion.x = nsevent.locationInWindow.x;
        event.motion.y = view.bounds.size.height - nsevent.locationInWindow.y;
        return event::motion_flag;

    case NSFlagsChanged:
        tuple = modifier( nsevent );

        if ( !tuple._0 )
            return event::ignore;

        event.key.value = tuple._0;
        event.key.press = tuple._1;
        return event::key_flag;

    case NSKeyDown:
    case NSKeyUp:
        event.key.value = [ nsevent.charactersIgnoringModifiers characterAtIndex: 0 ];
        event.key.press = nsevent.type == NSKeyDown;
        return event::key_flag;

    case NSLeftMouseDown:
    case NSLeftMouseUp:
    case NSRightMouseDown:
    case NSRightMouseUp:
    case NSOtherMouseDown:
    case NSOtherMouseUp:
        event.button.value = nsevent.buttonNumber;
        event.button.press = nsevent.type == NSLeftMouseDown || nsevent.type == NSRightMouseDown ||
            nsevent.type == NSOtherMouseDown;
        return event::button_flag;

    case NSScrollWheel:
        event.scroll.x = -nsevent.deltaX;
        event.scroll.y = -nsevent.deltaY;
        return event::scroll_flag;

    case NSEventTypeSwipe:
        event.swipe.x = nsevent.deltaX;
        event.swipe.y = nsevent.deltaY;
        return event::swipe_flag;

    case NSEventTypeMagnify:
        event.magnify.value = nsevent.magnification;
        return event::magnify_flag;

    case NSEventTypeRotate:
        event.rotate.value = nsevent.rotation;
        return event::rotate_flag;

    case NSEventTypeBeginGesture:
        return event::gesture_begin;

    case NSEventTypeEndGesture:
        return event::gesture_end;

    default:
        [ NSApp sendEvent: nsevent ];
        return event::ignore;
    }
}

OOE_NAMESPACE_END( ( ooe ) )
