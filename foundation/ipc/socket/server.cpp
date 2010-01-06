/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/executable/environment.hpp"
#include "foundation/ipc/io_buffer.hpp"
#include "foundation/ipc/socket/server.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe )( ipc )( socket ) )

OOE_TLS( ipc::socket::servlet* ) servlet_tls;

bool socket_read( ooe::socket& socket, io_buffer& buffer, up_t size )
{
	buffer.allocate( size );
	return socket.receive( buffer.get(), size ) == size;
}

bool socket_write( ooe::socket& socket, aligned_allocator& allocator, io_buffer& buffer, up_t size )
{
	if ( buffer.is_internal() )
	{
		if ( socket.send( buffer.get(), size ) == size )
			return true;
	}
	else
	{
		if ( socket.splice( allocator.get(), size ) == size )
			return true;
	}

	OOE_WARNING( "servlet: ", "Unable to write " << size << " bytes" );
	return false;
}

bool socket_write( ooe::socket& socket, u8* data, up_t size )
{
	if ( socket.send( data, size ) == size )
		return true;

	OOE_WARNING( "servlet: ", "Unable to write " << size << " bytes" );
	return false;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe )( ipc )( socket ) )

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( socket ) )

//--- servlet --------------------------------------------------------------------------------------
servlet::servlet( const servlet_iterator& iterator_, ooe::socket& socket_,
	const ipc::switchboard& switchboard_, server& server )
	: iterator( iterator_ ), socket( socket_ ), switchboard( switchboard_ ), state( true ),
	thread( make_function( *this, &servlet::call ), &server )
{
}

void servlet::join( void )
{
	socket.shutdown( ooe::socket::read_write );
	thread.join();
}

void servlet::migrate( ooe::socket& outgoing )
{
	outgoing.send( socket );
	state = false;
}

void* servlet::call( void* pointer )
{
	u8 data[ executable::static_page_size ];
	aligned_allocator allocator;
	io_buffer buffer( data, sizeof( data ), allocator );
	ipc::pool pool;

	u8 header[ sizeof( length_t ) + sizeof( index_t ) ];
	length_t length;
	index_t index;
	up_t preserve = stream_size< length_t, index_t >::call( length, index );
	servlet_tls = this;

	while ( state )
	{
		if ( OOE_UNLIKELY( !socket_read( socket, buffer, preserve ) ) )
			break;

		stream_read< length_t, index_t >::call( buffer.get(), length, index );

		if ( length && OOE_UNLIKELY( !socket_read( socket, buffer, length ) ) )
		{
			OOE_WARNING( "servlet: ", "Unable to read " << length << " bytes" );
			break;
		}

		switchboard::size_type size = switchboard.execute( index, buffer, pool );

		length = size._1 ? size._1 : size._0;
		error_t error = size._1 ? error::exception : error::none;
		stream_write< length_t, error_t >::call( header, length, error );

		if ( OOE_UNLIKELY( !socket_write( socket, header, preserve ) ) )
			break;
		else if ( length && OOE_UNLIKELY( !socket_write( socket, allocator, buffer, length ) ) )
			break;
	}

	static_cast< server* >( pointer )->erase( iterator );
	return 0;
}

//--- server ---------------------------------------------------------------------------------------
server::server( const address& address, const ipc::switchboard& switchboard_ )
	: listen( address ), switchboard( switchboard_ ), mutex(), list()
{
}

server::~server( void )
{
	for ( servlet_list::iterator i = list.begin(), end = list.end(); i != end; ++i )
		( *i )->join();
}

void server::accept( void )
{
	ooe::socket socket = listen.accept();

	lock lock( mutex );
	list.push_back( servlet_list::value_type() );
	list.back() = new servlet( --list.end(), socket, switchboard, *this );
}

void server::erase( const servlet_list::iterator& iterator )
{
	lock lock( mutex );
	list.erase( iterator );
}

void server::relink( ooe::socket& incoming )
{
	ooe::socket socket = incoming.receive();

	lock lock( mutex );
	list.push_back( servlet_list::value_type() );
	list.back() = new servlet( --list.end(), socket, switchboard, *this );
}

void server::migrate( ooe::socket& outgoing )
{
	servlet_tls->migrate( outgoing );
}

OOE_NAMESPACE_END( ( ooe )( ipc )( socket ) )
