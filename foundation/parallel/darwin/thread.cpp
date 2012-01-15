/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/parallel/thread.hpp"
#include "foundation/utility/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- platform -------------------------------------------------------------------------------------
void platform::thread_name( const std::string& name )
{
    s32 status = pthread_setname_np( name.c_str() );

    if ( status )
        OOE_CONSOLE( "thread \"" << name << "\": Unable to set name: " << error::number( status ) );
}

//--- thread ---------------------------------------------------------------------------------------
void thread::yield( void )
{
    pthread_yield_np();
}

OOE_NAMESPACE_END( ( ooe ) )
