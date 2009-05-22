/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/socket/client.hpp"

namespace ooe
{
//--- platform::ipc::socket::client --------------------------------------------
	platform::ipc::socket::client::client( void )
		: null( "/dev/null", ooe::descriptor::write )
	{
	}

	void platform::ipc::socket::client::splice( ooe::socket& socket, up_t size )
	{
		if ( socket.splice( null, size ) != size )
			throw error::runtime( "ipc::socket::client: " ) << "Unable to splice data";
	}
}
