/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/parallel/semaphore.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- semaphore_lock -------------------------------------------------------------------------------
semaphore_lock::semaphore_lock( ooe::semaphore& semaphore_ )
    : semaphore( semaphore_ )
{
    semaphore.down();
}

semaphore_lock::~semaphore_lock( void )
{
    semaphore.up();
}

OOE_NAMESPACE_END( ( ooe ) )
