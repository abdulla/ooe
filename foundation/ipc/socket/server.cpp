/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/socket/server.hpp"

namespace ooe
{
//--- nipc::servlet ------------------------------------------------------------
	nipc::servlet::servlet( const servlet_list::iterator& iterator_, ooe::socket& socket_,
		const nipc::switchboard& switchboard_, server& server )
		: iterator( iterator_ ), socket( socket_ ), switchboard( switchboard_ ),
		thread( make_function( *this, &servlet::call ), &server )
	{
	}

	void nipc::servlet::join( void )
	{
		socket.shutdown( socket::read_write );
		thread.join();
	}

	void* nipc::servlet::call( void* pointer )
	{
		buffer_type buffer;
		nipc::pool pool;

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

//--- nipc::server -------------------------------------------------------------
	nipc::server::server( const address& address )
		: listen( address ), mutex(), list()
	{
	}

	nipc::server::~server( void )
	{
		for ( servlet_list::iterator i = list.begin(), end = list.end(); i != end; ++i )
			( *i )->join();
	}

	void nipc::server::accept( const switchboard& switchboard )
	{
		socket socket = listen.accept();

		lock lock( mutex );
		list.push_back( servlet_list::value_type() );
		list.back() = new servlet( --list.end(), socket, switchboard, *this );
	}

	void nipc::server::erase( const servlet_list::iterator& iterator )
	{
		lock lock( mutex );
		list.erase( iterator );
	}
}
