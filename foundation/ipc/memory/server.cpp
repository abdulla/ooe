/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/ipc/name.hpp"
#include "foundation/ipc/memory/client.hpp"
#include "foundation/ipc/memory/server.hpp"
#include "foundation/utility/scoped.hpp"

namespace
{
	using namespace ooe;

	OOE_TLS( ipc::memory::servlet* ) servlet_tls;

	typedef tuple< const ipc::memory::switchboard&, ipc::memory::write_buffer*, ipc::pool* >
		tuple_type;

	void ipc_decode( u8* buffer_ptr, up_t buffer_size, const void* pointer )
	{
		const tuple_type& args = *static_cast< const tuple_type* >( pointer );
		args._0.execute( buffer_ptr, buffer_size, *args._1, *args._2 );
	}

	void ipc_link( const any& any, u8* buffer_ptr, up_t buffer_size,
		ipc::memory::write_buffer& buffer, ipc::pool& )
	{
		pid_t pid;
		time_t time;
		ipc::stream_read< pid_t, time_t >::call( header_adjust( buffer ), pid, time );

		u32 link = static_cast< ipc::memory::server* >( any.pointer )->link( pid, time );
		up_t size = ipc::stream_size< u32 >::call( link );
		u8* pointer = return_write( buffer_ptr, buffer_size, buffer, size );
		ipc::stream_write< u32 >::call( pointer, link );
	}

	void ipc_unlink( const any& any, u8* buffer_ptr, up_t buffer_size,
		ipc::memory::write_buffer& buffer, ipc::pool& )
	{
		u32 link;
		ipc::stream_read< u32 >::call( header_adjust( buffer ), link );

		static_cast< ipc::memory::server* >( any.pointer )->unlink( link );
		return_write( buffer_ptr, buffer_size, buffer );
	}

	void release( scoped_ptr< ipc::memory::link_server >& link )
	{
		delete link.release();
	}
}

namespace ooe
{
//--- ipc::memory::servlet -------------------------------------------------------------
	ipc::memory::servlet::servlet( const std::string& link_name, u32 link_id_,
		const memory::switchboard& switchboard_, server& server )
		: link_id( link_id_ ), transport( link_name, transport::create ),
		switchboard( switchboard_ ), listen( new link_listen( link_name ) ), link( 0 ),
		state( work ), thread( make_function( *this, &servlet::call ), &server )
	{
	}

	ipc::memory::servlet::servlet( socket& socket, u32 link_id_,
		const memory::switchboard& switchboard_, server& server )
		: link_id( link_id_ ), transport( socket ), switchboard( switchboard_ ), listen( 0 ),
		link( new link_server( socket, link_id, server ) ), state( work ),
		thread( make_function( *this, &servlet::call ), &server )
	{
		ipc::memory::shared_data& data =
			*static_cast< ipc::memory::shared_data* >( transport.private_data() );

		// disconnect from former server
		disconnect( data.name, data.link_id );

		// rewrite connection data in transport
		data.link_id = link_id;
		const std::string& name = server.name();
		std::memcpy( data.name, name.c_str(), name.size() + 1 );
	}

	ipc::memory::servlet::~servlet( void )
	{
		if ( state != move )
		{
			state = idle;
			stream_write< u32, u32 >::call( transport.get(), true, 0 );
			transport.wake_wait();
		}

		thread.join();
	}

	void* ipc::memory::servlet::call( void* pointer )
	{
		write_buffer buffer( transport.get(), 0, 0 );
		pool pool;
		tuple_type tuple( switchboard, &buffer, &pool );

		if ( listen )
		{
			memory::server& server = *static_cast< memory::server* >( pointer );
			const socket& socket = listen->accept();
			scoped_ptr< link_server >( new link_server( socket, link_id, server ) ).swap( link );
			delete listen.release();
			transport.unlink();
		}

		scoped< void ( scoped_ptr< link_server >& ) > scoped( release, link );
		servlet_tls = this;

		while ( state == work )
			transport.wait( ipc_decode, &tuple );

		return 0;
	}

	void ipc::memory::servlet::migrate( socket& socket )
	{
		transport.migrate( socket );
		link->migrate( socket );
		state = move;
	}

//--- ipc::memory::server --------------------------------------------------------------
	ipc::memory::server::server( const std::string& name_, const switchboard& external_ )
		: semaphore( name_, semaphore::create ), internal(), external( external_ ),
		transport( name_, transport::create ), seed(), servlets()
	{
		if ( internal.insert_direct( ipc_link, this ) != 1 )
			throw error::runtime( "ipc::memory::server: " ) << "\"link\" not at index 1";

		if ( internal.insert_direct( ipc_unlink, this ) != 2 )
			throw error::runtime( "ipc::memory::server: " ) << "\"unlink\" not at index 2";
	}

	// required to ensure servlet can be hidden
	ipc::memory::server::~server( void )
	{
	}

	std::string ipc::memory::server::name( void ) const
	{
		return transport.name();
	}

	bool ipc::memory::server::decode( void )
	{
		write_buffer buffer( transport.get(), 0, 0 );
		tuple_type tuple( internal, &buffer, 0 );
		transport.wait( ipc_decode, &tuple );
		return !servlets.empty();
	}

	// note: no need for lock or atomics, semaphore is used when calling link/unlink via decode
	u32 ipc::memory::server::link( pid_t pid, time_t time )
	{
		u32 link_id = seed++;
		std::string link_name = ipc::link_name( pid, time, link_id );
		servlet_map::value_type
			value( link_id, new servlet( link_name, link_id, external, *this ) );
		servlets.insert( servlets.end(), value );
		return link_id;
	}

	void ipc::memory::server::unlink( u32 link_id )
	{
		servlets.erase( link_id );
	}

	void ipc::memory::server::relink( socket& socket )
	{
		process_lock lock( semaphore );
		u32 link_id = seed++;
		servlet_map::value_type value( link_id, new servlet( socket, link_id, external, *this ) );
		servlets.insert( servlets.end(), value );
	}

	void ipc::memory::server::migrate( socket& socket )
	{
		( *servlet_tls ).migrate( socket );
	}
}
