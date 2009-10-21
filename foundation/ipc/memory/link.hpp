/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_LINK_HPP
#define OOE_FOUNDATION_IPC_MEMORY_LINK_HPP

#include "foundation/io/socket.hpp"
#include "foundation/ipc/memory/transport.hpp"
#include "foundation/parallel/thread.hpp"
#include "foundation/utility/atom.hpp"

namespace ooe
{
	namespace ipc
	{
		namespace memory
		{
			class server;
			class link_listen;
			class link_server;
			class link_client;
		}
	}

//--- ipc::memory::link_listen -------------------------------------------------
	class ipc::memory::link_listen
	{
	public:
		link_listen( const std::string& );
		~link_listen( void );

		ooe::socket accept( void ) const;

	private:
		mutable std::string path;
		ooe::listen listen;
	};

//--- ipc::memory::link_server -------------------------------------------------
	class ipc::memory::link_server
	{
	public:
		link_server( const ooe::socket&, u32, server& );
		~link_server( void );

		void migrate( ooe::socket& );

	private:
		ooe::socket socket;
		socket_pair migrate_pair;

		const u32 link_id;
		atom< u32 > active;
		ooe::thread thread;

		void* call( void* );
	};

//--- ipc::memory::link_client -------------------------------------------------
	class ipc::memory::link_client
	{
	public:
		link_client( const std::string&, transport& );
		~link_client( void );

		operator bool( void ) const;

	private:
		ooe::connect connect;
		atom< u32 > active;
		ooe::thread thread;

		void* call( void* );
	};
}

#endif	// OOE_FOUNDATION_IPC_MEMORY_LINK_HPP
