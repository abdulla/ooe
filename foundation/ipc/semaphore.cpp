/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <cerrno>

#include <fcntl.h>

#include "foundation/ipc/error.hpp"
#include "foundation/ipc/semaphore.hpp"

namespace ooe
{
//--- ipc::semaphore_id --------------------------------------------------------
	ipc::semaphore_id::semaphore_id( const std::string& name_ )
		: name( name_ )
	{
		if ( sem_unlink( name.c_str() ) && errno != OOE_NOEXIST )
			throw error::runtime( "semaphore: " ) <<
				"Unable to unlink semaphore \"" << name << "\": " << error::number( errno );
	}

	ipc::semaphore_id::~semaphore_id( void )
	{
		if ( sem_unlink( name.c_str() ) && errno != OOE_NOEXIST )
			OOE_WARNING( "semaphore",
				"Unable to unlink semaphore \"" << name << "\": " << error::number( errno ) );
	}

//--- ipc::semaphore -----------------------------------------------------------
	ipc::semaphore::semaphore( const std::string& name, type mode, u32 value )
		: id( mode == open ? 0 : new semaphore_id( name ) ),
		sem( sem_open( name.c_str(), mode == open ? 0 : O_CREAT | O_EXCL, 0600, value ) )
	{
		if ( sem == SEM_FAILED )
			throw error::runtime( "semaphore: " ) <<
				"Unable to open semaphore \"" << name << "\": " << error::number( errno );
	}

	ipc::semaphore::~semaphore( void )
	{
		if ( sem_close( sem ) )
			OOE_WARNING( "semaphore", "Unable to close semaphore: " << error::number( errno ) );
	}

	void ipc::semaphore::up( void )
	{
		if ( sem_post( sem ) )
			throw error::runtime( "semaphore: " ) <<
				"Unable to increment semaphore: " << error::number( errno );
	}

	void ipc::semaphore::down( void )
	{
		if ( sem_wait( sem ) )
			throw error::runtime( "semaphore: " ) <<
				"Unable to decrement semaphore: " << error::number( errno );
	}

	void ipc::semaphore::unlink( void )
	{
		delete id.release();
	}

//--- ipc::process_lock --------------------------------------------------------
	ipc::process_lock::process_lock( ipc::semaphore& semaphore_ )
		: semaphore( semaphore_ )
	{
		semaphore.down();
	}

	ipc::process_lock::~process_lock( void )
	{
		semaphore.up();
	}

//--- ipc::barrier_wait --------------------------------------------------------
	ipc::barrier_wait::barrier_wait( const std::string& name )
		: semaphore( name, ipc::semaphore::create, 0 )
	{
	}

	ipc::barrier_wait::~barrier_wait( void )
	{
		semaphore.down();
	}

//--- ipc ----------------------------------------------------------------------
	void ipc::barrier_notify( const std::string& name )
	{
		ipc::semaphore( name ).up();
	}
}
