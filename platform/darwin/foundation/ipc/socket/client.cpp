/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/socket/client.hpp"
#include "foundation/utility/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( platform )( ipc )( socket ) )

//--- client ---------------------------------------------------------------------------------------
void client::splice( ooe::socket& socket, up_t size )
{
	scoped_array< u8 > data( new u8[ size ] );

	if ( socket.receive( data, size ) != size )
		throw error::runtime( "ipc::socket::client: " ) << "Unable to splice " << size << " bytes";
}

OOE_NAMESPACE_END( ( ooe )( platform )( ipc )( socket ) )
