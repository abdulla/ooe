/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <mach/mach_init.h>
#include <mach/task.h>

#include "foundation/parallel/semaphore.hpp"
#include "foundation/utility/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

semaphore::semaphore( u32 value )
    : sem()
{
    if ( semaphore_create( mach_task_self(), &sem, SYNC_POLICY_FIFO, value ) )
        throw error::runtime( "semaphore: " ) << "Unable to initialise semaphore";
}

semaphore::~semaphore( void )
{
    if ( semaphore_destroy( mach_task_self(), sem ) )
        throw error::runtime( "semaphore: " ) << "Unable to destroy semaphore";
}

OOE_NAMESPACE_END( ( ooe )( platform ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

semaphore::semaphore( u32 value )
    : platform::semaphore( value )
{
}

semaphore::~semaphore( void )
{
}

void semaphore::up( void )
{
    if ( semaphore_signal( sem ) )
        throw error::runtime( "semaphore: " ) << "Unable to increment semaphore";
}

void semaphore::down( void )
{
    if ( semaphore_wait( sem ) )
        throw error::runtime( "semaphore: " ) << "Unable to decrement semaphore";
}

OOE_NAMESPACE_END( ( ooe ) )
