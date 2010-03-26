/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/timer.hpp"

namespace
{
	using namespace ooe;

	f32 difference( epoch_t begin, epoch_t end )
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
}
