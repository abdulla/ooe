/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_HPP
#define OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_HPP

#include "foundation/io/socket.hpp"
#include "foundation/ipc/shared_memory.hpp"
#include "foundation/ipc/memory/transport_forward.hpp"

namespace ooe
{
	namespace ipc
	{
		namespace memory
		{
			class transport;
		}
	}

//--- ipc::memory::transport ---------------------------------------------------
	class OOE_VISIBLE ipc::memory::transport
		: private platform::ipc::memory::transport
	{
	public:
		typedef void ( * wait_type )( u8*, up_t, const void* );

		enum type
		{
			open,
			create
		};

		transport( const std::string&, type );
		~transport( void );

		void wait( wait_type, const void* );
		void notify( void );
		void wake_wait( void );
		void wake_notify( void );

		u8* get( void ) const;
		up_t size( void ) const;
		void unlink( void );
		void migrate( ooe::socket& );

	private:
		shared_memory memory;
	};
}

#endif	// OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_HPP
