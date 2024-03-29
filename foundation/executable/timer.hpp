/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_EXECUTABLE_TIMER_HPP
#define OOE_FOUNDATION_EXECUTABLE_TIMER_HPP

#include <ctime>

#include "foundation/utility/fundamental.hpp"
#include "foundation/utility/macro.hpp"
#include "foundation/utility/tuple.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

typedef tuple< std::time_t /* seconds */, std::time_t /* nanoseconds */ > epoch_t;

class OOE_VISIBLE timer
{
public:
    timer( void );

    f32 update( void );
    f32 elapsed( void ) const;
    epoch_t get( void ) const;

    static epoch_t epoch( void );
    static epoch_t sleep( epoch_t );

private:
    epoch_t time;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_EXECUTABLE_TIMER_HPP
