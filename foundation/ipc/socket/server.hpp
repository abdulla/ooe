/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_SOCKET_SERVER_HPP
#define OOE_FOUNDATION_IPC_SOCKET_SERVER_HPP

#include <list>

#include "foundation/io/socket.hpp"
#include "foundation/ipc/socket/switchboard.hpp"
#include "foundation/parallel/lock.hpp"
#include "foundation/parallel/thread.hpp"

namespace ooe
{
	namespace ipc
	{
		namespace socket
		{
			class servlet;
			class server;

			typedef shared_ptr< servlet > servlet_ptr;
			typedef std::list< servlet_ptr > servlet_list;
			typedef std::list< servlet_ptr >::iterator servlet_iterator;
		}
	}

//--- ipc::socket::servlet -----------------------------------------------------
	class ipc::socket::servlet
	{
	public:
		servlet( const servlet_iterator&, ooe::socket&, const socket::switchboard&, server& );

		void join( void );

	private:
		servlet_iterator iterator;
		ooe::socket socket;
		const socket::switchboard& switchboard;
		ooe::thread thread;

		void* call( void* );
	};

//--- ipc::socket::server ------------------------------------------------------
	class OOE_VISIBLE ipc::socket::server
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
