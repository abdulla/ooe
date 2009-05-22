/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/socket/client.hpp"

namespace ooe
{
//--- platform::ipc::socket::client --------------------------------------------
	void platform::ipc::socket::client::splice( ooe::socket& socket, up_t size )
	{
		ooe::ipc::socket::header_read( socket, size );
	}
}
