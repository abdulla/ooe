/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/io/socket.hpp"

namespace ooe
{
//--- platform::socket ---------------------------------------------------------
	platform::socket::socket( s32 fd )
		: descriptor( fd )
	{
	}
}
