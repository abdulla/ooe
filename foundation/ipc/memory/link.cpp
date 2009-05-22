/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <cerrno>

#include "foundation/ipc/memory/error.hpp"
#include "foundation/ipc/memory/link.hpp"
#include "foundation/ipc/memory/server.hpp"
#include "foundation/utility/convert.hpp"

namespace ooe
{
//--- ipc::link_listen ---------------------------------------------------------
	ipc::link_listen::link_listen( const std::string& name )
		: path( local_name( name ) ), listen( local( path ) )
	{
	}

	ipc::link_listen::~link_listen( void )
	{
		if ( path.size() && unlink( path.c_str() ) )
			OOE_WARNING( "ipc::link_listen",
				"Unable to unlink \"" << path << "\": " << error::number( errno ) );
	}

	socket ipc::link_listen::accept( void ) const
	{
		socket socket = listen.accept();

		if ( unlink( path.c_str() ) )
			throw error::runtime( "ipc::link_server: " ) <<
				"Unable to unlink \"" << path << "\": " << error::number( errno );

		path = std::string();	// required to force deallocation of string
		return socket;
	}

//--- ipc::link_server ---------------------------------------------------------
	ipc::link_server::link_server( const ooe::socket& socket_, u32 link_id_, server& server )
		: socket( socket_ ), link_id( link_id_ ), active( true ),
		thread( make_function( *this, &link_server::call ), &server )
	{
	}

	ipc::link_server::~link_server( void )
	{
		if ( !active )
			return;

		active = false;
		socket.shutdown( socket::read );
		thread.join();
	}

	void* ipc::link_server::call( void* pointer )
	{
		ipc::server& server = *static_cast< ipc::server* >( pointer );
		socket.poll();

		if ( !active )
			return 0;

		active = false;
		server.unlink( link_id );
		return 0;
	}

//--- ipc::link_client ---------------------------------------------------------
	ipc::link_client::link_client( const std::string& name, transport& transport )
		: connect( local( local_name( name ) ) ), active( true ),
		thread( make_function( *this, &link_client::call ), &transport )
	{
	}

	ipc::link_client::~link_client( void )
	{
		if ( !active )
			return;

		active = false;
		connect.shutdown( socket::read );
		thread.join();
	}

	ipc::link_client::operator bool( void ) const
	{
		return active;
	}

	void* ipc::link_client::call( void* pointer )
	{
		ipc::transport& transport = *static_cast< ipc::transport* >( pointer );
		connect.poll();

		if ( !active )
			return 0;

		stream_write< u32, u32 >::call( transport.get()._0, true, error::link );
		transport.wake_notify();
		active = false;
		return 0;
	}
}
