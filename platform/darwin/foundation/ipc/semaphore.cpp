/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/io/descriptor.hpp"
#include "foundation/ipc/semaphore.hpp"

namespace ooe
{
//--- descriptor ---------------------------------------------------------------
	template<>
		s32 descriptor::illegal_access< true >( void ) const
	{
		return id->fd;
	}

//--- ipc::semaphore -----------------------------------------------------------
	ipc::semaphore::semaphore( const descriptor& desc_ )
		: id( 0 ), sem( reinterpret_cast< sem_t* >( desc_.illegal_access< true >() ) )
	{
	}

	descriptor ipc::semaphore::desc( void ) const
	{
		return reinterpret_cast< up_t >( sem );
	}
}
