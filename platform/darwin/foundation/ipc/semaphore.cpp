/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/io/descriptor.hpp"
#include "foundation/ipc/semaphore.hpp"

namespace
{
	sem_t* create_semaphore( const descriptor& desc )
	{
		s32 fd = dup( desc.get() );

		if ( fd == -1 )
			throw error::runtime( "semaphore: " ) <<
				"Unable to duplicate descriptor: " << error::number( errno );
		else if ( fcntl( outfd, F_SETFD, FD_CLOEXEC ) )
			throw error::runtime( "semaphore: " ) <<
				"Unable to set close-on-exec: " << error::number( errno );

		return reinterpret_cast< sem_t* >( fd );
	}
}

namespace ooe
{
//--- ipc::semaphore -----------------------------------------------------------
	ipc::semaphore::semaphore( const descriptor& desc )
		: id( 0 ), sem( create_semaphore( desc ) )
	{
	}

	ipc::semaphore::operator descriptor( void ) const
	{
		return dup( reinterpret_cast< up_t >( sem ) );
	}
}
