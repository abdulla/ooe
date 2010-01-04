/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/socket/client.hpp"
#include "foundation/utility/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( platform )( ipc )( socket ) )

//--- client ---------------------------------------------------------------------------------------
client::client( void )
	: null( "/dev/null", ooe::descriptor::write )
{
}

void client::splice( ooe::socket& socket, up_t size )
{
	if ( socket.splice( null, size ) != size )
		throw error::runtime( "ipc::socket::client: " ) << "Unable to splice " << size << " bytes";
}

OOE_NAMESPACE_END( ( ooe )( platform )( ipc )( socket ) )
