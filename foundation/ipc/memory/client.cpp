/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/ipc/name.hpp"
#include "foundation/ipc/semaphore.hpp"
#include "foundation/ipc/memory/client.hpp"
#include "foundation/ipc/memory/rpc_forward.hpp"

namespace
{
	using namespace ooe;

	u32 ipc_connect( const std::string& name )
	{
		ipc::memory::transport transport( name, ipc::memory::transport::open );
		ipc::semaphore semaphore( name, ipc::semaphore::open );
		ipc::process_lock lock( semaphore );

		ipc::memory::rpc< u32 ( pid_t ) > link( transport, 1 );
		return link( getpid() );
	}

	void ipc_disconnect( const std::string& name, u32 link )
	{
		ipc::memory::transport transport( name, ipc::memory::transport::open );
		ipc::semaphore semaphore( name, ipc::semaphore::open );
		ipc::process_lock lock( semaphore );

		ipc::memory::rpc< void ( u32 ) > unlink( transport, 2 );
		unlink( link );
	}

	ipc::memory::transport* create_transport( const std::string& name )
	{
		if ( name.size() + 1 > sizeof( ipc::memory::shared_data ) - sizeof( u32 ) )
			throw error::runtime( "ipc::memory::client: " ) << '"' << name << "\" is too long";

		u32 link_id = ipc_connect( name );
		std::string link_name = ipc::link_name( getpid(), link_id );
		scoped_ptr< ipc::memory::transport >
			transport( new ipc::memory::transport( link_name, ipc::memory::transport::open ) );

		ipc::memory::shared_data& data =
			*static_cast< ipc::memory::shared_data* >( transport->private_data() );
		data.link_id = link_id;
		std::memcpy( data.name, name.c_str(), name.size() + 1 );

		return transport.release();
	}

	std::string create_link( const ipc::memory::transport& transport )
	{
		u32 link_id = static_cast< ipc::memory::shared_data* >( transport.private_data() )->link_id;
		return ipc::link_name( getpid(), link_id );
	}
}

namespace ooe
{
//--- ipc::memory::client ------------------------------------------------------
	ipc::memory::client::client( const std::string& name )
		: transport( create_transport( name ) ), link( create_link( *transport ), *transport )
	{
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
