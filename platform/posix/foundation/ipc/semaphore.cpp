/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cerrno>

#include "foundation/ipc/semaphore.hpp"
#include "foundation/utility/error.hpp"

namespace ooe
{
//--- platform::ipc::unnamed_semaphore -----------------------------------------
	platform::ipc::unnamed_semaphore::unnamed_semaphore( u32 value )
		: sem()
	{
		if ( sem_init( &sem, true, value ) )
			throw error::runtime( "unnamed_semaphore: " ) << "Unable to initialise semaphore";
	}

	platform::ipc::unnamed_semaphore::~unnamed_semaphore( void )
	{
		if ( sem_destroy( &sem ) )
			throw error::runtime( "unnamed_semaphore: " ) << "Unable to destroy semaphore";
	}

	void platform::ipc::unnamed_semaphore::up( void )
	{
		if ( sem_post( &sem ) )
			throw error::runtime( "semaphore: " ) <<
				"Unable to increment semaphore: " << error::number( errno );
	}

	void platform::ipc::unnamed_semaphore::down( void )
	{
		if ( sem_wait( &sem ) )
			throw error::runtime( "semaphore: " ) <<
				"Unable to decrement semaphore: " << error::number( errno );
	}
}
