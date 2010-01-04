/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cerrno>

#include <mach/mach_time.h>

#include "foundation/executable/timer.hpp"
#include "foundation/utility/error.hpp"

namespace
{
	using namespace ooe;
	mach_timebase_info_data_t timebase = { 0, 0 };

	void initialise_timebase( void )
	{
		if ( !timebase.denom && mach_timebase_info( &timebase ) )
			throw error::runtime( "timer: " ) <<
			"Unable to get time base: " << error::number( errno );
	}
}

namespace ooe
{
//--- timer --------------------------------------------------------------------
	epoch_t timer::epoch( void )
	{
		initialise_timebase();
		u64 time = mach_absolute_time() * timebase.numer / timebase.denom;
		return epoch_t( time / 1000000000, time % 1000000000 );
	}

	epoch_t timer::sleep( const epoch_t& value )
	{
		initialise_timebase();
		u64 absolute = mach_absolute_time() +
			( value._0 * 1000000000 + value._1 ) * timebase.numer / timebase.denom;

		if ( mach_wait_until( absolute ) )
			throw error::runtime( "timer: " ) << "Unable to sleep: " << error::number( errno );

		return epoch_t( 0, 0 );
	}
}
