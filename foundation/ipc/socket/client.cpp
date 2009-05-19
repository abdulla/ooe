/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/socket/client.hpp"

namespace ooe
{
//--- ipc::socket::client ------------------------------------------------------
	ipc::socket::client::client( const address& address )
		: connect( address ), buffer(), map(), in( 0 ), out( 0 ), mutex(), condition(),
		thread( make_function( *this, &client::call ), 0 )
	{
	}

	ipc::socket::client::~client( void )
	{
		rpc< void ( void ) >( *this, 0 )()();	// call null function to flush pending calls
		connect.shutdown( socket::read_write );	// shutdown socket to stop reader thread
		thread.join();
	}

	ipc::socket::array_type ipc::socket::client::wait( result_type& result )
	{
		lock lock( mutex );

		while ( result.i->first > in )
			condition.wait( lock );

		array_type array = result.i->second._0;
		result.state = result.i->second._1 ? result_type::done : result_type::error;
		map.erase( result.i );
		return array;
	}

	void ipc::socket::client::erase( const iterator_type& i )
	{
		lock lock( mutex );
		map.erase( i );
	}

	ipc::socket::client::iterator_type ipc::socket::client::insert( void )
	{
		lock lock( mutex );
		return map.insert( map.end(), map_type::value_type( ++out, map_tuple() ) );
	}

	ipc::buffer_tuple ipc::socket::get( void ) const
	{
		return buffer_tuple( buffer + sizeof( u32 ), sizeof( buffer ) - sizeof( u32 ) );
	}

	void ipc::socket::write( const void* buffer, up_t size )
	{
		*( u32* )buffer = size;
		socket.write( buffer, size + sizeof( u32 ) );
	}

	void* ipc::socket::client::call( void* )
	{
		while ( true )
		{
			bool done = true;
			u32 size;

			if ( !header_size( connect, size ) )
				break;
			else if ( size == static_cast< u32 >( -1 ) )
			{
				// an exception occurred, read exception
				if ( !header_size( connect, size ) )
					break;

				done = false;
			}

			array_type array = header_read( connect, size );

			{
				lock lock( mutex );
				map_type::iterator i = map.find( ++in );

				if ( i != map.end() )
					i->second = map_tuple( array, done );
			}

			condition.notify_all();
		}

		return 0;
	}
}
