/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/socket/client.hpp"
#include "foundation/ipc/socket/rpc.hpp"

namespace ooe
{
//--- ipc::socket::client ------------------------------------------------------
	ipc::socket::client::client( const address& address )
		: platform::ipc::socket::client(), connect( address ), buffer(), map(), in( 0 ), out( 0 ),
		notify( 0 ), mutex(), condition(), thread( make_function( *this, &client::call ), 0 )
	{
	}

	ipc::socket::client::~client( void )
	{
		rpc< void ( void ) >( *this, 0 )()();			// call null function to flush pending calls
		connect.shutdown( ooe::socket::read_write );	// shutdown socket to stop reader thread
		thread.join();
	}

	ipc::socket::array_type ipc::socket::client::wait( result_type& result )
	{
		map_tuple tuple;

		{
			lock lock( mutex );

			if ( result.i->first > in )
			{
				notify = result.i->first;
				condition.wait( lock );
			}

			tuple = result.i->second;
			map.erase( result.i );
		}

		result.state = tuple._1 ? result_type::done : result_type::error;
		return tuple._0;
	}

	void ipc::socket::client::erase( const iterator& i )
	{
		lock lock( mutex );
		map.erase( i );
	}

	ipc::socket::client::iterator ipc::socket::client::insert( void )
	{
		lock lock( mutex );
		return map.insert( map.end(), map_type::value_type( ++out, map_tuple() ) );
	}

	u8* ipc::socket::client::get( void ) const
	{
		return buffer;
	}

	up_t ipc::socket::client::size( void ) const
	{
		return sizeof( buffer );
	}

	void ipc::socket::client::write( const u8* data, up_t length )
	{
		if ( connect.send( data, length ) != length )
			throw error::runtime( "ipc::socket::client: " ) << "Unable to write data";
	}

	void* ipc::socket::client::call( void* )
	{
		while ( true )
		{
			bool done = true;
			u32 length;

			if ( !header_size( connect, length ) )
				break;
			else if ( length == ~u32( 0 ) )
			{
				// an exception occurred, read exception
				if ( !header_size( connect, length ) )
					break;

				done = false;
			}

			bool do_splice = false;

			{
				lock lock( mutex );
				map_type::iterator i = map.find( ++in );

				if ( i != map.end() )
					i->second = map_tuple( header_read( connect, length ), done );
				else
					do_splice = true;
			}

			if ( in == notify )
				condition.notify_one();

			if ( do_splice )
				splice( connect, length );	// data not needed, splice out of stream
		}

		return 0;
	}
}
