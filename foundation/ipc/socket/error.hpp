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
		const bool executed;

		socket_rpc( bool executed_ )
			: runtime( "ipc::socket::rpc: " ), executed( executed_ )
		{
			*this << "\nexecuted: " << executed << '\n';
		}

		virtual ~socket_rpc( void ) throw()
		{
		}
	};
}

#endif	// OOE_FOUNDATION_IPC_SOCKET_ERROR_HPP
