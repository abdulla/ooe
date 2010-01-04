/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/io/socket.hpp"

namespace ooe
{
//--- platform::socket ---------------------------------------------------------
	platform::socket::socket( const descriptor& fd )
		: descriptor( fd )
	{
	}
}
