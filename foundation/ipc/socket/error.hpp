/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_SOCKET_ERROR_HPP
#define OOE_FOUNDATION_IPC_SOCKET_ERROR_HPP

#include "foundation/utility/error.hpp"

namespace ooe
{
	namespace error
	{
		struct socket_rpc;
	}

	struct OOE_VISIBLE error::socket_rpc
		: virtual public runtime
	{
		socket_rpc( void )
			: runtime( "ipc::socket::rpc: " )
		{
		}

		virtual ~socket_rpc( void ) throw()
		{
		}
	};
}

#endif	// OOE_FOUNDATION_IPC_SOCKET_ERROR_HPP
