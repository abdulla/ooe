/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/ipc/name.hpp"
#include "foundation/ipc/memory/server.hpp"

namespace
{
	using namespace ooe;

	typedef tuple< const ipc::memory::switchboard&, ipc::memory::write_buffer*, ipc::pool* >
		tuple_type;

	void ipc_decode( const ipc::memory::buffer_tuple& tuple, const void* pointer )
	{
		const tuple_type& args = *static_cast< const tuple_type* >( pointer );
		args._0.execute( tuple, *args._1, *args._2 );
	}

	void ipc_link( const any& any, const ipc::memory::buffer_tuple& tuple,
		ipc::memory::write_buffer& buffer, ipc::pool& )
	{
		pid_t pid;
		ipc::stream_read< pid_t >::call( header_adjust( buffer ), pid );

		u32 link = static_cast< ipc::memory::server* >( any.pointer )->link( pid );
		up_t size = ipc::stream_size< u32 >::call( link );
		ipc::stream_write< u32 >::call( return_write( tuple, buffer, size ), link );
	}

	void ipc_unlink( const any& any, const ipc::memory::buffer_tuple& tuple,
		ipc::memory::write_buffer& buffer, ipc::pool& )
	{
		u32 link;
		ipc::stream_read< u32 >::call( header_adjust( buffer ), link );

		static_cast< ipc::memory::server* >( any.pointer )->unlink( link );
		return_write( tuple, buffer );
	}
}

namespace ooe
{
//--- ipc::memory::servlet -------------------------------------------------------------
	ipc::memory::servlet::servlet( pid_t pid, u32 link_id_, transport_type type,
		const memory::switchboard& switchboard_, server& server )
		: link_id( link_id_ ), transport( type( link_name( pid, link_id ), transport::create ) ),
		switchboard( switchboard_ ), listen( new link_listen( link_name( pid, link_id ) ) ),
		active( true ), thread( make_function( *this, &servlet::call ), &server )
	{
	}

	ipc::memory::servlet::~servlet( void )
	{
		ooe::thread self;

		if ( thread == self )
			return;

		active = false;
		stream_write< u32, u32 >::call( transport->get()._0, true, 0 );
		transport->wake_wait();
		thread.join();
	}

	void* ipc::memory::servlet::call( void* pointer )
	{
		write_buffer buffer( transport->get(), 0 );
		pool pool;
		tuple_type tuple( switchboard, &buffer, &pool );

		memory::server& server = *static_cast< memory::server* >( pointer );
		link_server link( listen->accept(), link_id, server );
		delete listen.release();
		transport->unlink();

		while ( active )
			transport->wait( ipc_decode, &tuple );

		return 0;
	}

//--- ipc::memory::server --------------------------------------------------------------
	ipc::memory::server::
		server( transport_type type_, const std::string& name, const switchboard& external_ )
		: semaphore( name + ".s", semaphore::create ), internal(), external( external_ ),
		type( type_ ), transport( type( name, transport::create ) ), seed(), servlets()
	{
		if ( internal.insert_direct( ipc_link, this ) != 1 )
			throw error::runtime( "ipc::server: " ) << "\"link\" not at index 1";

		if ( internal.insert_direct( ipc_unlink, this ) != 2 )
			throw error::runtime( "ipc::server: " ) << "\"unlink\" not at index 2";
	}

	// note: so that ~servlet() does not need to be visible
	ipc::memory::server::~server( void )
	{
	}

	bool ipc::memory::server::decode( void )
	{
		write_buffer buffer( transport->get(), 0 );
		tuple_type tuple( internal, &buffer, 0 );
		transport->wait( ipc_decode, &tuple );
		return !servlets.empty();
	}

	// note: no need for lock or atomics, semaphore is used when calling link/unlink via decode
	u32 ipc::memory::server::link( pid_t pid )
	{
		u32 link_id = seed++;
		servlet_map::value_type
			value( link_id, new servlet( pid, link_id, type, external, *this ) );
		servlets.insert( servlets.end(), value );
		return link_id;
	}

	void ipc::memory::server::unlink( u32 link_id )
	{
		servlets.erase( link_id );
	}
}
