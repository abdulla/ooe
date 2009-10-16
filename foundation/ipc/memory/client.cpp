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
		ipc::semaphore semaphore( name + ".s", ipc::semaphore::open );
		ipc::process_lock lock( semaphore );

		ipc::memory::rpc< u32 ( pid_t ) > link( transport, 1 );
		return link( getpid() );
	}

	void ipc_disconnect( const std::string& name, u32 link )
	{
		ipc::memory::transport transport( name, ipc::memory::transport::open );
		ipc::semaphore semaphore( name + ".s", ipc::semaphore::open );
		ipc::process_lock lock( semaphore );

		ipc::memory::rpc< void ( u32 ) > unlink( transport, 2 );
		unlink( link );
	}
}

namespace ooe
{
//--- ipc::memory::client ------------------------------------------------------
	ipc::memory::client::client( const std::string& name_ )
		: name( name_ ), link_id( ipc_connect( name ) ),
		transport( link_name( getpid(), link_id ), transport::open ),
		link( link_name( getpid(), link_id ), transport )
	{
	}

	ipc::memory::client::~client( void )
	{
		if ( link )
			OOE_PRINT( "ipc::client", ipc_disconnect( name, link_id ) );
	}

	ipc::memory::client::operator memory::transport&( void )
	{
		return transport;
	}
}
