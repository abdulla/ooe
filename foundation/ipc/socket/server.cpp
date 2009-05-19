/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/socket/header.hpp"
#include "foundation/ipc/socket/server.hpp"

namespace ooe
{
//--- ipc::socket::servlet -----------------------------------------------------
	ipc::socket::servlet::servlet( const servlet_list::iterator& iterator_, ooe::socket& socket_,
		const socket::switchboard& switchboard_, server& server )
		: iterator( iterator_ ), socket( socket_ ), switchboard( switchboard_ ),
		thread( make_function( *this, &servlet::call ), &server )
	{
	}

	void ipc::socket::servlet::join( void )
	{
		socket.shutdown( ooe::socket::read_write );
		thread.join();
	}

	void* ipc::socket::servlet::call( void* pointer )
	{
		buffer_type buffer;
		ipc::pool pool;

		while ( true )
		{
			u32 size;

			if ( !header_size( socket, size ) )
				break;

			header_read( socket, size, buffer );
			switchboard.execute( &buffer[ 0 ], socket, pool );
		}

		static_cast< server* >( pointer )->erase( iterator );
		return 0;
	}

//--- ipc::socket::server ------------------------------------------------------
	ipc::socket::server::server( const address& address )
		: listen( address ), mutex(), list()
	{
	}

	ipc::socket::server::~server( void )
	{
		for ( servlet_list::iterator i = list.begin(), end = list.end(); i != end; ++i )
			( *i )->join();
	}

	void ipc::socket::server::accept( const switchboard& switchboard )
	{
		ooe::socket socket = listen.accept();

		lock lock( mutex );
		list.push_back( servlet_list::value_type() );
		list.back() = new servlet( --list.end(), socket, switchboard, *this );
	}

	void ipc::socket::server::erase( const servlet_list::iterator& iterator )
	{
		lock lock( mutex );
		list.erase( iterator );
	}
}
