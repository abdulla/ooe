/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/parallel/thread.hpp"

namespace ooe
{
	void thread::yield( void )
	{
		pthread_yield();
	}
}
