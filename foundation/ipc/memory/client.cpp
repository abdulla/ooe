/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/ipc/memory/client.hpp"
#include "foundation/ipc/memory/name.hpp"
#include "foundation/ipc/memory/rpc_forward.hpp"
#include "foundation/ipc/memory/semaphore.hpp"

namespace
{
	using namespace ooe;

	u32 ipc_connect( ipc::transport_type type, const std::string& name )
	{
		scoped_ptr< ipc::transport > transport( type( name, ipc::transport::open ) );
		ipc::semaphore semaphore( name + ".s", ipc::semaphore::open );
		ipc::process_lock lock( semaphore );

		ipc::rpc< u32 ( pid_t ) > link( *transport, 1 );
		return link( getpid() );
	}

	void ipc_disconnect( ipc::transport_type type, const std::string& name, u32 link )
	{
		scoped_ptr< ipc::transport > transport( type( name, ipc::transport::open ) );
		ipc::semaphore semaphore( name + ".s", ipc::semaphore::open );
		ipc::process_lock lock( semaphore );

		ipc::rpc< void ( u32 ) > unlink( *transport, 2 );
		unlink( link );
	}
}

namespace ooe
{
//--- ipc::client --------------------------------------------------------------
	ipc::client::client( transport_type type_, const std::string& name_ )
		: type( type_ ), name( name_ ), link_id( ipc_connect( type, name ) ),
		transport( type( link_name( getpid(), link_id ), transport::open ) ),
		link( link_name( getpid(), link_id ), *transport )
	{
	}

	ipc::client::~client( void )
	{
		if ( link )
			OOE_PRINT( "ipc::client", ipc_disconnect( type, name, link_id ) );
	}

	ipc::client::operator ipc::transport&( void )
	{
		return *transport;
	}
}
