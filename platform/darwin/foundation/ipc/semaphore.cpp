/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/io/descriptor.hpp"
#include "foundation/ipc/semaphore.hpp"

namespace ooe
{
//--- ipc::semaphore -----------------------------------------------------------
	ipc::semaphore::semaphore( const descriptor& desc )
		: id( 0 ), sem( reinterpret_cast< sem_t* >( desc.get() ) )
	{
	}

	ipc::semaphore::operator descriptor( void ) const
	{
		return reinterpret_cast< up_t >( sem );
	}
}
