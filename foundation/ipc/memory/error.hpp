/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_ERROR_HPP
#define OOE_FOUNDATION_IPC_MEMORY_ERROR_HPP

#include "foundation/utility/error.hpp"

namespace ooe
{
	namespace error
	{
		enum ipc
		{
			none,
			link,
			exception
		};

		struct rpc;
		struct connection;
	}

	struct OOE_VISIBLE error::rpc
		: virtual public runtime
	{
		rpc( void )
			: runtime( "ipc::rpc: " )
		{
		}

		virtual ~rpc( void ) throw()
		{
		}
	};

	struct OOE_VISIBLE error::connection
		: virtual public runtime
	{
		connection( void )
			: runtime( "ipc::rpc: " )
		{
			*this << "Connection down";
		}

		virtual ~connection( void ) throw()
		{
		}
	};
}

#endif	// OOE_FOUNDATION_IPC_MEMORY_ERROR_HPP
