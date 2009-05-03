/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_LINK_HPP
#define OOE_FOUNDATION_IPC_LINK_HPP

#include "foundation/io/socket.hpp"
#include "foundation/ipc/transport.hpp"
#include "foundation/parallel/thread.hpp"
#include "foundation/utility/atom.hpp"

namespace ooe
{
	namespace ipc
	{
		class server;
		class link_listen;
		class link_server;
		class link_client;
	}

//--- ipc::link_listen ---------------------------------------------------------
	class ipc::link_listen
	{
	public:
		link_listen( const std::string& );
		~link_listen( void );

		socket accept( void ) const;

	private:
		mutable std::string path;
		ooe::listen listen;
	};

//--- ipc::link_server ---------------------------------------------------------
	class ipc::link_server
	{
	public:
		link_server( const ooe::socket&, u32, server& );
		~link_server( void );

	private:
		ooe::socket socket;
		const u32 link_id;
		atom<> active;
		ooe::thread thread;

		void* call( void* );
	};

//--- ipc::link_client ---------------------------------------------------------
	class ipc::link_client
	{
	public:
		link_client( const std::string&, transport& );
		~link_client( void );

		operator bool( void ) const;

	private:
		ooe::connect connect;
		atom<> active;
		ooe::thread thread;

		void* call( void* );
	};
}

#endif	// OOE_FOUNDATION_IPC_LINK_HPP
