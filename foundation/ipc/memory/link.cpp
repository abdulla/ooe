/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <cerrno>

#include "foundation/io/poll.hpp"
#include "foundation/ipc/memory/link.hpp"
#include "foundation/ipc/memory/server.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

//--- link_listen ----------------------------------------------------------------------------------
link_listen::link_listen( const std::string& name )
	: path( local_name( name ) ), listen( local_address( path ) )
{
}

link_listen::~link_listen( void )
{
	if ( unlink( path.c_str() ) )
		OOE_WARNING( "ipc::link_listen",
			"Unable to unlink \"" << path << "\": " << error::number( errno ) );
}

socket link_listen::accept( void ) const
{
	return listen.accept();
}

//--- link_server ----------------------------------------------------------------------------------
link_server::link_server( const ooe::socket& socket_, link_t link_, server& server )
	: socket( socket_ ), pair( make_pair() ), link( link_ ), state( work ),
	thread( make_function( *this, &link_server::main ), &server )
{
}

link_server::~link_server( void )
{
	if ( state == idle )
		return;
	else if ( state == work )
	{
		state = idle;
		socket.shutdown( socket::read );
	}

	thread.join();
}

void link_server::migrate( ooe::socket& migrate_socket )
{
	state = move;
	migrate_socket.send( socket );
	pair._1.shutdown( socket::write );
}

void* link_server::main( void* pointer )
{
	memory::server& server = *static_cast< memory::server* >( pointer );

	poll poll;
	poll.insert( socket );
	poll.insert( pair._0 );
	poll.wait();

	if ( state != work )
		return 0;

	state = idle;
	server.unlink( link, true );
	return 0;
}

//--- link_client ----------------------------------------------------------------------------------
link_client::link_client( const std::string& name, transport& transport )
	: connect( local_address( local_name( name ) ) ), state( true ),
	thread( make_function( *this, &link_client::main ), &transport )
{
}

link_client::~link_client( void )
{
	if ( !state )
		return;

	state = false;
	connect.shutdown( socket::read );
	thread.join();
}

link_client::operator bool( void ) const
{
	return state;
}

void* link_client::main( void* pointer )
{
	memory::transport& transport = *static_cast< memory::transport* >( pointer );

	poll poll;
	poll.insert( connect );
	poll.wait();

	if ( !state )
		return 0;

	// wake for result, indicating that an error in the link has occurred
	stream_write< bool_t, error_t, const c8* >::call( transport.get(), true, error::link, "" );
	transport.wake_notify();
	state = false;
	return 0;
}

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )
