/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_ERROR_HPP
#define OOE_FOUNDATION_IPC_ERROR_HPP

#include "foundation/ipc/error_forward.hpp"
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
		struct verification;
	}

	struct OOE_VISIBLE error::rpc
		: virtual public runtime
	{
		const bool executed;

		rpc( bool executed_ )
			: runtime( "ipc: " ), executed( executed_ )
		{
			using ooe::operator <<;
			*this << "Executed: " << executed << '\n';
		}

		virtual ~rpc( void ) throw()
		{
		}
	};

	struct OOE_VISIBLE error::connection
		: virtual public runtime
	{
		connection( void )
			: runtime( "ipc: " )
		{
			*this << "Connection down";
		}

		virtual ~connection( void ) throw()
		{
		}
	};

	struct OOE_VISIBLE error::verification
		: virtual public runtime
	{
		verification( const void* pointer, u8 index )
			: runtime( "ipc: " )
		{
			using ooe::operator <<;
			*this << "Invalid pointer " << pointer << " in argument " << index;
		}

		virtual ~verification( void ) throw()
		{
		}
	};
}

#endif	// OOE_FOUNDATION_IPC_ERROR_HPP
