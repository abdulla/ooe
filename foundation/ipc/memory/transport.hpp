/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_HPP
#define OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_HPP

#include "foundation/utility/fundamental.hpp"
#include "foundation/utility/macro.hpp"
#include "foundation/utility/tuple.hpp"

namespace ooe
{
	namespace ipc
	{
		struct transport;
	}

//--- ipc::transport -----------------------------------------------------------
	struct ipc::transport
	{
		typedef tuple< u8* /* buffer */, up_t /* size */ > tuple_type;
		typedef void ( * wait_type )( transport&, const void* );

		enum type
		{
			open,
			create
		};

		virtual ~transport( void ) {}
		virtual void wait( wait_type, const void* ) = 0;
		virtual void notify( void ) = 0;
		virtual tuple_type get( void ) = 0;

		virtual void wake_wait( void ) = 0;
		virtual void wake_notify( void ) = 0;
		virtual void unlink( void ) = 0;
	};

	namespace ipc
	{
//--- ipc ----------------------------------------------------------------------
		typedef transport* ( * transport_type )( const std::string&, transport::type );

		transport* create_spinlock( const std::string&, transport::type ) OOE_VISIBLE;
		transport* create_semaphore( const std::string&, transport::type ) OOE_VISIBLE;
	}
}

#endif	// OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_HPP
