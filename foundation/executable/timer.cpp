/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/timer.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

f32 difference( epoch_t begin, epoch_t end )
{
    return end._0 - begin._0 + ( end._1 - begin._1 ) * 1e-9f;
}

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- timer ----------------------------------------------------------------------------------------
timer::timer( void )
    : time( epoch() )
{
}

f32 timer::update( void )
{
    epoch_t now = epoch();
    f32 seconds = difference( time, now );
    time = now;
    return seconds;
}

f32 timer::elapsed( void ) const
{
    return difference( time, epoch() );
}

epoch_t timer::get( void ) const
{
    return time;
}

OOE_NAMESPACE_END( ( ooe ) )
