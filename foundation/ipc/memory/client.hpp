/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_CLIENT_HPP
#define OOE_FOUNDATION_IPC_CLIENT_HPP

#include "foundation/ipc/link.hpp"

namespace ooe
{
	namespace ipc
	{
		class client;
	}

//--- ipc::client --------------------------------------------------------------
	class OOE_VISIBLE ipc::client
	{
	public:
		client( transport_type, const std::string& );
		~client( void );

		operator ipc::transport&( void );

	private:
		transport_type type;
		const std::string name;
		const u32 link_id;

		const scoped_ptr< ipc::transport > transport;
		link_client link;
	};
}

#endif	// OOE_FOUNDATION_IPC_CLIENT_HPP
