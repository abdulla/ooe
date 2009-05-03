/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_SOCKET_SERVER_HPP
#define OOE_FOUNDATION_IPC_SOCKET_SERVER_HPP

#include <list>

#include "foundation/io/socket.hpp"
#include "foundation/parallel/lock.hpp"
#include "foundation/parallel/thread.hpp"

#include "foundation/ipc/socket/header.hpp"
#include "foundation/ipc/socket/switchboard.hpp"

namespace ooe
{
	namespace nipc
	{
		class servlet;
		class server;

		typedef shared_ptr< servlet > servlet_type;
		typedef std::list< servlet_type > servlet_list;
		typedef std::list< servlet_type >::iterator servlet_iterator;
	}

//--- nipc::servlet ------------------------------------------------------------
	class nipc::servlet
	{
	public:
		servlet( const servlet_iterator&, ooe::socket&, const nipc::switchboard&, server& );

		void join( void );

	private:
		servlet_iterator iterator;
		ooe::socket socket;
		const nipc::switchboard& switchboard;
		ooe::thread thread;

		void* call( void* );
	};

//--- nipc::server -------------------------------------------------------------
	class OOE_VISIBLE nipc::server
	{
	public:
		server( const address& );
		~server( void );

		void accept( const switchboard& );
		void erase( const servlet_iterator& ) OOE_HIDDEN;

	private:
		ooe::listen listen;
		ooe::mutex mutex;
		servlet_list list;
	};
}

#endif	// OOE_FOUNDATION_IPC_SOCKET_SERVER_HPP
