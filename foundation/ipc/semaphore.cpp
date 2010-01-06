/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <cerrno>

#include <fcntl.h>

#include "foundation/ipc/error.hpp"
#include "foundation/ipc/semaphore.hpp"

namespace
{
	using namespace ooe;

	void sem_remove( const std::string& name )
	{
		if ( sem_unlink( name.c_str() ) && errno != OOE_NOEXIST )
			throw error::runtime( "ipc::semaphore: " ) <<
				"Unable to unlink semaphore \"" << name << "\": " << error::number( errno );
	}

	sem_t* sem_create( const std::string& name, bool create, u32 value )
	{
		if ( create )
			sem_remove( name );

		s32 flags = create ? O_CREAT | O_EXCL: 0;
		sem_t* sem = sem_open( name.c_str(), flags, 0600, value );

		if ( sem == SEM_FAILED )
			throw error::runtime( "ipc::semaphore: " ) <<
				"Unable to open semaphore \"" << name << "\": " << error::number( errno );

		return sem;
	}

	void sem_destroy( const std::string& name, bool unlinkable, sem_t* sem )
	{
		if ( sem_close( sem ) )
			OOE_WARNING( "ipc::semaphore",
				"Unable to close semaphore \"" << name << "\": " << error::number( errno ) );

		if ( unlinkable && sem_unlink( name.c_str() ) && errno != OOE_NOEXIST )
			OOE_WARNING( "ipc::semaphore",
				"Unable to unlink semaphore \"" << name << "\": " << error::number( errno ) );
	}
}

namespace ooe
{
//--- ipc::semaphore -----------------------------------------------------------
	ipc::semaphore::semaphore( const std::string& sem_name, type mode, u32 value )
		: name_( sem_name ), unlinkable( mode == create ),
		sem( sem_create( name_, mode == create, value ) )
	{
	}

	ipc::semaphore::~semaphore( void )
	{
		sem_destroy( name_, unlinkable, sem );
	}

	void ipc::semaphore::up( void )
	{
		if ( sem_post( sem ) )
			throw error::runtime( "ipc::semaphore: " ) <<
				"Unable to increment semaphore: " << error::number( errno );
	}

	void ipc::semaphore::down( void )
	{
		if ( sem_wait( sem ) )
			throw error::runtime( "ipc::semaphore: " ) <<
				"Unable to decrement semaphore: " << error::number( errno );
	}

	std::string ipc::semaphore::name( void ) const
	{
		return name_;
	}

	void ipc::semaphore::unlink( void )
	{
		if ( unlinkable )
		{
			sem_remove( name_ );
			name_ = std::string();
			unlinkable = false;
		}
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
