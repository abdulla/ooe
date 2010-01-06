/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_SOCKET_CLIENT_FORWARD_HPP
#define OOE_FOUNDATION_IPC_SOCKET_CLIENT_FORWARD_HPP

#include "foundation/io/socket.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( platform )( ipc )( socket ) )

class client
{
protected:
	void splice( ooe::socket&, up_t );
};

OOE_NAMESPACE_END( ( ooe )( platform )( ipc )( socket ) )

#endif	// OOE_FOUNDATION_IPC_SOCKET_CLIENT_FORWARD_HPP
