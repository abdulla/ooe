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
		}
	}

//--- ipc::memory::client ------------------------------------------------------
	class OOE_VISIBLE ipc::memory::client
	{
	public:
		client( transport_type, const std::string& );
		~client( void );

		operator memory::transport&( void );

	private:
		transport_type type;
		const std::string name;
		const u32 link_id;

		const scoped_ptr< memory::transport > transport;
		link_client link;
	};
}

#endif	// OOE_FOUNDATION_IPC_MEMORY_CLIENT_HPP
