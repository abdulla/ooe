/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <cerrno>

#include <sys/time.h>

#include "foundation/executable/timer.hpp"
#include "foundation/utility/error.hpp"

namespace
{
	using namespace ooe;

	f32 difference( const epoch_t& begin, const epoch_t& end )
	{
		return static_cast< f32 >( end._0 - begin._0 ) +
			static_cast< f32 >( end._1 - begin._1 ) * 1e-9f;
	}
}

namespace ooe
{
//--- timer --------------------------------------------------------------------
	timer::timer( void )
		: time( epoch() )
	{
	}

	f32 timer::update( void )
	{
		epoch_t now = epoch();
		f32 seconds = difference( time, now );
		time = now;
		return seconds;
	}

	f32 timer::elapsed( void ) const
	{
		return difference( time, epoch() );
	}

	epoch_t timer::get( void ) const
	{
		return time;
	}

	epoch_t timer::epoch( void )
	{
		timeval value;

		if ( gettimeofday( &value, 0 ) )
			throw error::runtime( "timer: " ) << "Unable to get UTC: " << error::number( errno );

		return epoch_t( value.tv_sec, value.tv_usec * 1000 );
	}

	epoch_t timer::sleep( const epoch_t& time )
	{
		epoch_t period( time );

		if ( period._1 >= 1000000000 )
		{
			period._0 += period._1 / 1000000000;
			period._1 %= 1000000000;
		}

		timespec* spec = reinterpret_cast< timespec* >( &period );

		if ( nanosleep( spec, spec ) )
			throw error::runtime( "timer: " ) << "Unable to sleep: " << error::number( errno );

		return period;
	}
}
