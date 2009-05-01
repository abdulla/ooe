/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "external/nipc/client.hpp"
#include "external/nipc/rpc_forward.hpp"

namespace ooe
{
//--- nipc::client -------------------------------------------------------------
	nipc::client::client( const address& address )
		: connect( address ), map(), in( 0 ), out( 0 ), mutex(), condition(),
		thread( make_function( *this, &client::call ), 0 )
	{
	}

	nipc::client::~client( void )
	{
		rpc< void ( void ) >( *this, 0 )()();	// call null function to flush pending calls
		connect.shutdown( socket::read_write );	// shutdown socket to stop reader thread
		thread.join();
	}

	nipc::array_type nipc::client::wait( result_base< void >& result )
	{
		lock lock( mutex );

		while ( result.i->first > in )
			condition.wait( lock );

		array_type array = result.i->second._0;
		map.erase( result.i );
		result.state = result.i->second._1 ? result_base< void >::done : result_base< void >::error;
		return array;
	}

	void nipc::client::erase( const map_type::iterator& i )
	{
		lock lock( mutex );
		map.erase( i );
	}

	nipc::client::map_type::iterator nipc::client::insert( void )
	{
		lock lock( mutex );
		return map.insert( map.end(), map_type::value_type( ++out, map_tuple() ) );
	}

	nipc::client::operator socket&( void )
	{
		return connect;
	}

	void* nipc::client::call( void* )
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
