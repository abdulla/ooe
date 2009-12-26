/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/executable/timer.hpp"
#include "foundation/ipc/name.hpp"
#include "foundation/ipc/semaphore.hpp"
#include "foundation/ipc/memory/client.hpp"
#include "foundation/ipc/memory/rpc_forward.hpp"

namespace
{
	using namespace ooe;

	u32 ipc_connect( const std::string& name, pid_t pid, time_t time )
	{
		ipc::memory::transport transport( name, ipc::memory::transport::open );
		ipc::semaphore semaphore( name, ipc::semaphore::open );
		ipc::process_lock lock( semaphore );

		ipc::memory::rpc< u32 ( pid_t, time_t ) > link( transport, 1 );
		return link( pid, time );
	}

	void ipc_disconnect( const std::string& name, u32 link )
	{
		ipc::memory::transport transport( name, ipc::memory::transport::open );
		ipc::semaphore semaphore( name, ipc::semaphore::open );
		ipc::process_lock lock( semaphore );

		ipc::memory::rpc< void ( u32 ) > unlink( transport, 2 );
		unlink( link );
	}
}

namespace ooe
{
//--- ipc::memory::client ------------------------------------------------------
	ipc::memory::client::client( const std::string& name )
		: transport( 0 ), link( 0 )
	{
		if ( name.size() + 1 > sizeof( ipc::memory::shared_data ) - sizeof( u32 ) )
			throw error::runtime( "ipc::memory::client: " ) << '"' << name << "\" is too long";

		pid_t pid = getpid();
		time_t time = timer::epoch()._0;
		u32 link_id = ipc_connect( name, pid, time );
		std::string link_name = ipc::link_name( pid, time, link_id );

		transport_ptr( new ipc::memory::transport( link_name, ipc::memory::transport::open ) )
			.swap( transport );
		link_ptr( new link_client( link_name, *transport ) ).swap( link );

		// set private data, used for migration
		ipc::memory::shared_data& data =
			*static_cast< ipc::memory::shared_data* >( transport->private_data() );
		data.link_id = link_id;
		std::memcpy( data.name, name.c_str(), name.size() + 1 );
	}

	ipc::memory::client::~client( void )
	{
		if ( link )
		{
			shared_data& data = *static_cast< shared_data* >( transport->private_data() );
			OOE_PRINT( "ipc::memory::client", ipc_disconnect( data.name, data.link_id ) );
		}
	}

	ipc::memory::client::operator memory::transport&( void )
	{
		return *transport;
	}

	void ipc::memory::disconnect( const std::string& name, u32 link_id )
	{
		ipc_disconnect( name, link_id );
	}
}
