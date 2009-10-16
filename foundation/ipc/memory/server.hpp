/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_SERVER_HPP
#define OOE_FOUNDATION_IPC_MEMORY_SERVER_HPP

#include <map>

#include "foundation/ipc/semaphore.hpp"
#include "foundation/ipc/memory/switchboard.hpp"
#include "foundation/ipc/memory/link.hpp"

namespace ooe
{
	namespace ipc
	{
		namespace memory
		{
			class servlet;
			class server;
		}
	}

//--- ipc::memory::servlet -----------------------------------------------------
	class ipc::memory::servlet
	{
	public:
		servlet( pid_t, u32, const memory::switchboard&, server& );
		~servlet( void );

	private:
		const u32 link_id;
		memory::transport transport;
		const memory::switchboard& switchboard;

		scoped_ptr< const link_listen > listen;
		atom< u32 > active;
		ooe::thread thread;

		void* call( void* );
	};

//--- ipc::memory::server ------------------------------------------------------
	class ipc::memory::server
	{
	public:
		server( const std::string&, const switchboard& ) OOE_VISIBLE;

		bool decode( void ) OOE_VISIBLE;

		u32 link( pid_t );
		void unlink( u32 );

	private:
		typedef std::map< u32, shared_ptr< servlet > > servlet_map;

		ipc::semaphore semaphore;
		memory::switchboard internal;
		const memory::switchboard& external;
		memory::transport transport;

		u32 seed;
		servlet_map servlets;
	};
}

#endif	// OOE_FOUNDATION_IPC_MEMORY_SERVER_HPP
