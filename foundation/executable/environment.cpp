/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <cerrno>

#include <sched.h>
#include <unistd.h>

#include "foundation/executable/environment.hpp"
#include "foundation/utility/error.hpp"

namespace ooe
{
	void executable::yield( void )
	{
		if ( sched_yield() )
			throw error::runtime( "executable: " ) << "Unable to yield process: " <<
				error::number( errno );
	}

	up_t executable::cpu_cores( void )
	{
		sp_t value = sysconf( _SC_NPROCESSORS_ONLN );

		if ( value == -1 )
			throw error::runtime( "executable: " ) << "Unable to get number of cpu cores: " <<
				error::number( errno );

		return value;
	}

	up_t executable::page_size( void )
	{
		sp_t value = sysconf( _SC_PAGESIZE );

		if ( value == -1 )
			throw error::runtime( "executable: " ) << "Unable to get page size: " <<
				error::number( errno );

		return value;
	}
}

