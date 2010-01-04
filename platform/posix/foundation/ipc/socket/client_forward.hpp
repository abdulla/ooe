/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_SOCKET_CLIENT_FORWARD_HPP
#define OOE_FOUNDATION_IPC_SOCKET_CLIENT_FORWARD_HPP

#include "foundation/io/socket.hpp"

namespace ooe
{
	namespace platform
	{
		namespace ipc
		{
			namespace socket
			{
				class client;
			}
		}
	}

	class platform::ipc::socket::client
	{
	protected:
		ooe::descriptor null;

		client( void );

		void splice( ooe::socket&, up_t );
	};
}

#endif	// OOE_FOUNDATION_IPC_SOCKET_CLIENT_FORWARD_HPP
