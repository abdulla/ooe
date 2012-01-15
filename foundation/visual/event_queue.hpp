/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_VISUAL_EVENT_QUEUE_HPP
#define OOE_FOUNDATION_VISUAL_EVENT_QUEUE_HPP

#include "foundation/executable/timer.hpp"

#include OOE_PATH( foundation/visual, event_queue_forward.hpp )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- event ----------------------------------------------------------------------------------------
struct event
{
    enum type
    {
        none,
        exit,
        ignore,

        motion_flag,
        button_flag,
        key_flag,

        scroll_flag,
        swipe_flag,
        magnify_flag,
        rotate_flag,

        gesture_begin,
        gesture_end
    };

    union
    {
        struct
        {
            f32 x;
            f32 y;
        } motion, scroll, swipe;

        struct
        {
            u32 value;
            bool press;
        } button, key;

        struct
        {
            f32 value;
        } magnify, rotate;
    };
};

//--- event_queue ----------------------------------------------------------------------------------
struct OOE_VISIBLE event_queue
    : private noncopyable, public platform::event_queue
{
    event::type dequeue( event&, epoch_t = epoch_t( 0, 0 ) ) const;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_VISUAL_EVENT_QUEUE_HPP
