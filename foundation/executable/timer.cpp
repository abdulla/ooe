/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <cerrno>

#include <sys/time.h>

#include "foundation/executable/timer.hpp"
#include "foundation/utility/error.hpp"

namespace ooe
{
//--- timer --------------------------------------------------------------------
	timer::timer( void )
		: time()
	{
		epoch( time );
	}

	f32 timer::elapsed( void )
	{
		epoch_t now;
		epoch( now );

		f32 seconds = static_cast< f32 >( now._0 - time._0 ) +
			static_cast< f32 >( now._1 - time._1 ) * 1e-9f;

		time = now;
		return seconds;
	}

	epoch_t timer::get( void ) const
	{
		return time;
	}

	void timer::epoch( epoch_t& time )
	{
		if ( gettimeofday( reinterpret_cast< timeval* >( &time ), 0 ) )
			throw error::runtime( "timer: " ) << "Unable to get UTC: " << error::number( errno );

		time._1 *= 1000;
	}

	void timer::sleep( epoch_t& time )
	{
		if ( time._1 >= 1000000000 )
		{
			time._0 += time._1 / 1000000000;
			time._1 %= 1000000000;
		}

		timespec* spec = reinterpret_cast< timespec* >( &time );

		if ( nanosleep( spec, spec ) )
			throw error::runtime( "timer: " ) << "Unable to sleep: " << error::number( errno );
	}
}
