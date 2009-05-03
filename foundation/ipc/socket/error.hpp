/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_SOCKET_ERROR_HPP
#define OOE_FOUNDATION_IPC_SOCKET_ERROR_HPP

#include "foundation/utility/error.hpp"

namespace ooe
{
	namespace error
	{
		struct nrpc;
	}

	struct OOE_VISIBLE error::nrpc
		: virtual public runtime
	{
		nrpc( void )
			: runtime( "nipc::rpc: " )
		{
		}

		virtual ~nrpc( void ) throw()
		{
		}
	};
}

#endif	// OOE_FOUNDATION_IPC_SOCKET_ERROR_HPP
