/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <sys/socket.h>

#include "foundation/io/socket.hpp"

namespace ooe
{
//--- platform::socket ---------------------------------------------------------
	platform::socket::socket( s32 fd )
		: descriptor( fd )
	{
		static_cast< ooe::socket* >( this )->option( SO_NOSIGPIPE, true );
	}
}
