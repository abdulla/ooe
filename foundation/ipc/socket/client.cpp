/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/pool.hpp"
#include "foundation/ipc/socket/client.hpp"
#include "foundation/ipc/socket/rpc.hpp"
#include "foundation/utility/scoped.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe )( ipc )( socket ) )

bool socket_read( ooe::socket& socket, io_buffer& buffer, up_t size )
{
	buffer.allocate( size );
	return socket.receive( buffer.get(), size ) == size;
}

void error_set( u64& notify, mutex& mutex, condition& condition )
{
	{
		lock lock( mutex );
		notify = ~u64( 0 );
	}

	condition.notify_one();
}

void error_check( u64 notify )
{
	if ( OOE_UNLIKELY( notify == ~u64( 0 ) ) )
		throw error::connection();
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe )( ipc )( socket ) )

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( socket ) )

//--- client ---------------------------------------------------------------------------------------
client::client( const address& address )
	: platform::ipc::socket::client(), connect( address ), map(), in( 0 ), out( 0 ), notify( 0 ),
	mutex(), condition(), thread( make_function( *this, &client::call ), 0 ), scratch()
{
}

client::~client( void )
{
	if ( notify != ~u64( 0 ) )
	{
		rpc< void ( void ) >( *this, 0 )()();			// call null function to flush pending calls
		connect.shutdown( ooe::socket::read_write );	// shutdown socket to stop reader thread
	}

	thread.join();
}

client::array_type client::wait( result_type& result )
{
	map_tuple tuple;

	{
		lock lock( mutex );
		error_check( notify );

		if ( result.i->first > in )
		{
			notify = result.i->first;
			condition.wait( lock );
			error_check( notify );
		}

		tuple = result.i->second;
		map.erase( result.i );
	}

	result.state = tuple._1 ? result_type::done : result_type::error;
	return tuple._0;
}

void client::erase( const iterator& i )
{
	lock lock( mutex );
	map.erase( i );
}

client::iterator client::insert( void )
{
	lock lock( mutex );
	return map.insert( map.end(), map_type::value_type( ++out, map_tuple() ) );
}

u8* client::get( void ) const
{
	return scratch;
}

up_t client::size( void ) const
{
	return sizeof( scratch );
}

void client::write( const u8* data, up_t size_ )
{
	// TODO: need to use vmsplice() here
	if ( connect.send( data, size_ ) != size_ )
		throw error::runtime( "ipc::socket::client: " ) << "Unable to write " << size_ << " bytes";
}

void* client::call( void* )
{
	scoped< void ( u64&, ooe::mutex&, ooe::condition& ) >
		scoped( error_set, notify, mutex, condition );

	heap_allocator allocator;
	io_buffer buffer( 0, 0, allocator );
	ipc::pool pool;

	u8 header[ sizeof( length_t ) + sizeof( index_t ) ];
	length_t length;
	error_t error;
	up_t preserve = stream_size< length_t, error_t >::call( length, error );

	while ( true )
	{
		if ( OOE_UNLIKELY( connect.receive( header, preserve ) != preserve ) )
			break;

		stream_read< length_t, error_t >::call( header, length, error );
		bool do_splice = false;
		bool do_notify;

		{
			lock lock( mutex );
			map_type::iterator i = map.find( ++in );
			do_notify = in == notify;

			if ( i == map.end() )
				do_splice = true;
			else if ( length && OOE_UNLIKELY( !socket_read( connect, buffer, length ) ) )
				throw error::runtime( "ipc::socket::client: " ) <<
					"Unable to read " << length << " bytes";
			else
				i->second = map_tuple( allocator.release(), error == error::none );
		}

		if ( do_splice )
			splice( connect, length );	// data not needed, splice out of stream
		else if ( do_notify )
			condition.notify_one();
	}

	return 0;
}

OOE_NAMESPACE_END( ( ooe )( ipc )( socket ) )
