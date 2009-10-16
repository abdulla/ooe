/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_ERROR_HPP
#define OOE_FOUNDATION_IPC_ERROR_HPP

#include "foundation/io/socket.hpp"
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

	namespace ipc
	{
		class migration;
	}

	struct OOE_VISIBLE error::rpc
		: virtual public runtime
	{
		const bool executed;

		rpc( bool executed_ )
			: runtime( "ipc: " ), executed( executed_ )
		{
			*this << "Executed: " << ( executed ? "true" : "false" ) << '\n';
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
		verification( const void* value, u8 index )
			: runtime( "ipc: " )
		{
			using ooe::operator <<;
			*this << "Invalid pointer " << ptr( value ) << " in argument " << index;
		}

		virtual ~verification( void ) throw()
		{
		}
	};

	class OOE_VISIBLE ipc::migration
	{
	public:
		migration( const ooe::address& in )
			: address_( in )
		{
		}

		ooe::address address( void ) const
		{
			return address_;
		}

	private:
		ooe::address address_;
	};
}

#endif	// OOE_FOUNDATION_IPC_ERROR_HPP
