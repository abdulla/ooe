/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_CLIENT_HPP
#define OOE_FOUNDATION_IPC_MEMORY_CLIENT_HPP

#include "foundation/ipc/memory/link.hpp"

namespace ooe
{
	namespace ipc
	{
		namespace memory
		{
			class client;
			struct shared_data;
			void disconnect( const std::string&, u32 );
		}
	}

//--- ipc::memory::client ------------------------------------------------------
	class OOE_VISIBLE ipc::memory::client
	{
	public:
		client( const std::string& );
		~client( void );

		operator memory::transport&( void );

	private:
		typedef scoped_ptr< memory::transport > transport_ptr;
		typedef scoped_ptr< link_client > link_ptr;

		transport_ptr transport;
		link_ptr link;
	};

//--- ipc::memory::shared_data -------------------------------------------------
	struct ipc::memory::shared_data
	{
		u32 link_id;
		c8 name[ ipc::memory::transport::private_size - sizeof( u32 ) ];
	} OOE_PACKED;
}

#endif	// OOE_FOUNDATION_IPC_MEMORY_CLIENT_HPP
