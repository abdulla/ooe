/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/ipc/name.hpp"
#include "foundation/ipc/pool.hpp"
#include "foundation/ipc/server.hpp"
#include "foundation/ipc/traits.hpp"

namespace
{
	using namespace ooe;

	typedef tuple< const ipc::switchboard&, ipc::memory_type*, ipc::pool* > tuple_type;

	void ipc_decode( ipc::transport& transport, const void* pointer )
	{
		ipc::transport::tuple_type t0 = transport.get();
		const tuple_type& t1 = *static_cast< const tuple_type* >( pointer );
		t1._0.execute( t0._0, t0._1, *t1._1, *t1._2 );
	}

	void ipc_link( any any, const ipc::buffer_unpack& buffer_unpack, ipc::buffer_pack& buffer_pack,
		ipc::pool& )
	{
		pid_t pid;
		ipc::layout_unpack< pid_t >::call( buffer_unpack, pid );
		u32 link = static_cast< ipc::server* >( any.pointer )->link( pid );
		ipc::layout_pack< u32 >::call( buffer_pack, link );
	}

	void ipc_unlink( any any, const ipc::buffer_unpack& buffer_unpack, ipc::buffer_pack&,
		ipc::pool& )
	{
		u32 link;
		ipc::layout_unpack< u32 >::call( buffer_unpack, link );
		static_cast< ipc::server* >( any.pointer )->unlink( link );
	}
}

namespace ooe
{
//--- ipc::servlet -------------------------------------------------------------
	ipc::servlet::servlet( pid_t pid, u32 link_id_, transport_type type,
		const ipc::switchboard& switchboard_, server& server )
		: link_id( link_id_ ), transport( type( link_name( pid, link_id ), transport::create ) ),
		switchboard( switchboard_ ), listen( new link_listen( link_name( pid, link_id ) ) ),
		go( 1 ), thread( make_function( *this, &servlet::call ), &server )
	{
	}

	ipc::servlet::~servlet( void )
	{
		ooe::thread self;

		if ( thread == self )
			return;

		go = 0;
		transport::tuple_type tuple = transport->get();
		header_type& header = *reinterpret_cast< header_type* >( tuple._0 );
		header._0 = 0;		// call null function to shutdown
		header._1 = false;	// not using rpc to avoid check
		transport->wake_wait();
		thread.join();
	}

	void* ipc::servlet::call( void* pointer )
	{
		memory_type memory( 0 );
		pool pool;
		tuple_type tuple( switchboard, &memory, &pool );

		ipc::server& server = *static_cast< ipc::server* >( pointer );
		link_server link( listen->accept(), link_id, server );
		delete listen.release();
		transport->unlink();

		while ( go )
			transport->wait( ipc_decode, &tuple );

		return 0;
	}

//--- ipc::server --------------------------------------------------------------
	ipc::server::
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
	ipc::server::~server( void )
	{
	}

	bool ipc::server::decode( void )
	{
		memory_type memory( 0 );
		tuple_type tuple( internal, &memory, 0 );
		transport->wait( ipc_decode, &tuple );
		return !servlets.empty();
	}

	// note: no need for lock or atomics, semaphore is used when calling link/unlink via decode
	u32 ipc::server::link( pid_t pid )
	{
		u32 link_id = seed++;
		servlet_map::value_type
			value( link_id, new servlet( pid, link_id, type, external, *this ) );
		servlets.insert( servlets.end(), value );
		return link_id;
	}

	void ipc::server::unlink( u32 link_id )
	{
		servlets.erase( link_id );
	}
}
