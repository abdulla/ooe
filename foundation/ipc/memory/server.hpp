/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_SERVER_HPP
#define OOE_FOUNDATION_IPC_MEMORY_SERVER_HPP

#include <map>

#include "foundation/ipc/memory/switchboard.hpp"
#include "foundation/ipc/memory/semaphore.hpp"
#include "foundation/ipc/memory/link.hpp"

namespace ooe
{
	namespace ipc
	{
		class servlet;
		class server;
	}

//--- ipc::servlet -------------------------------------------------------------
	class ipc::servlet
	{
	public:
		servlet( pid_t, u32, transport_type, const ipc::switchboard&, server& );
		~servlet( void );

	private:
		const u32 link_id;
		const scoped_ptr< ipc::transport > transport;
		const ipc::switchboard& switchboard;

		scoped_ptr< const link_listen > listen;
		atom<> active;
		ooe::thread thread;

		void* call( void* );
	};

//--- ipc::server --------------------------------------------------------------
	class ipc::server
	{
	public:
		server( transport_type, const std::string&, const switchboard& ) OOE_VISIBLE;
		~server( void ) OOE_VISIBLE;

		bool decode( void ) OOE_VISIBLE;

		u32 link( pid_t );
		void unlink( u32 );

	private:
		typedef std::map< u32, shared_ptr< servlet > > servlet_map;

		ipc::semaphore semaphore;
		ipc::switchboard internal;
		const ipc::switchboard& external;
		const transport_type type;
		const scoped_ptr< ipc::transport > transport;

		u32 seed;
		servlet_map servlets;
	};
}

#endif	// OOE_FOUNDATION_IPC_MEMORY_SERVER_HPP
