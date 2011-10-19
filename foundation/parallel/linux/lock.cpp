/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cerrno>

#include "foundation/parallel/lock.hpp"
#include "foundation/utility/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

semaphore::semaphore( u32 value )
    : sem()
{
    if ( sem_init( &sem, true, value ) )
        throw error::runtime( "semaphore: " ) << "Unable to initialise semaphore";
}

semaphore::~semaphore( void )
{
    if ( sem_destroy( &sem ) )
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
    if ( sem_post( &sem ) )
        throw error::runtime( "semaphore: " ) <<
            "Unable to increment semaphore: " << error::number( errno );
}

void semaphore::down( void )
{
    if ( sem_wait( &sem ) )
        throw error::runtime( "semaphore: " ) <<
            "Unable to decrement semaphore: " << error::number( errno );
}

OOE_NAMESPACE_END( ( ooe ) )
