/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_PLATFORM_DARWIN_FOUNDATION_VISUAL_EVENT_QUEUE_FORWARD_HPP
#define OOE_PLATFORM_DARWIN_FOUNDATION_VISUAL_EVENT_QUEUE_FORWARD_HPP

#include "foundation/utility/namespace.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

enum key_type
{
    key_escape          = 0x001b,
    key_return          = 0x000d,
    key_backspace       = 0x007f,
    key_shift_left      = 0xffe1,
    key_shift_right     = 0xffe2,
    key_command_left    = 0xffeb,
    key_command_right   = 0xffec,

    key_up              = 0xf700,
    key_down            = 0xf701,
    key_left            = 0xf702,
    key_right           = 0xf703
};

OOE_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

class event_queue
{
protected:
    event_queue( void ) OOE_VISIBLE;
};

OOE_NAMESPACE_END( ( ooe )( platform ) )

#endif  // OOE_PLATFORM_DARWIN_FOUNDATION_VISUAL_EVENT_QUEUE_FORWARD_HPP
