/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <cerrno>
#include <ctime>

#include "foundation/executable/timer.hpp"
#include "foundation/utility/error.hpp"

namespace ooe
{
//--- timer --------------------------------------------------------------------
	epoch_t timer::epoch( void )
	{
		timespec value;

		if ( clock_gettime( CLOCK_MONOTONIC, &value ) )
			throw error::runtime( "timer: " ) << "Unable to get time: " << error::number( errno );

		return epoch_t( value.tv_sec, value.tv_nsec );
	}

	epoch_t timer::sleep( const epoch_t& value )
	{
		timespec request = { value._0, value._1 };

		if ( request.tv_nsec >= 1000000000 )
		{
			request.tv_sec += request.tv_nsec / 1000000000;
			request.tv_nsec %= 1000000000;
		}

		timespec remain;

		if ( clock_nanosleep( CLOCK_MONOTONIC, 0, &request, &remain ) )
			throw error::runtime( "timer: " ) << "Unable to sleep: " << error::number( errno );

		return epoch_t( remain.tv_sec, remain.tv_nsec );
	}
}
