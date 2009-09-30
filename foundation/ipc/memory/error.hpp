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

		struct memory_rpc;
		struct connection;
	}

	struct OOE_VISIBLE error::memory_rpc
		: virtual public runtime
	{
		const bool executed;

		memory_rpc( bool executed_ )
			: runtime( "ipc::memory::rpc: " ), executed( executed_ )
		{
			*this << "Executed: " << ( executed ? "true" : "false" ) << '\n';
		}

		virtual ~memory_rpc( void ) throw()
		{
		}
	};

	struct OOE_VISIBLE error::connection
		: virtual public runtime
	{
		connection( void )
			: runtime( "ipc::memory::rpc: " )
		{
			*this << "Connection down";
		}

		virtual ~connection( void ) throw()
		{
		}
	};
}

#endif	// OOE_FOUNDATION_IPC_MEMORY_ERROR_HPP
