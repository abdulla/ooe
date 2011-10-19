/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/parallel/thread.hpp"
#include "foundation/utility/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- platform -------------------------------------------------------------------------------------
void platform::thread_name( const std::string& name )
{
    s32 status = pthread_setname_np( pthread_self(), name.c_str() );

    if ( status )
        OOE_CONSOLE( "thread \"" << name << "\": Unable to set name: " << error::number( status ) );
}

//--- thread ---------------------------------------------------------------------------------------
void thread::yield( void )
{
    pthread_yield();
}

OOE_NAMESPACE_END( ( ooe ) )
