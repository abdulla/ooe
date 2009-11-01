/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "component/registry/registry.hpp"
#include "foundation/ipc/memory/rpc.hpp"

namespace ooe
{
//--- registry -----------------------------------------------------------------
	registry::registry( void )
		: client( "/ooe.registry" )
	{
	}

	registry::info_vector registry::find( const interface& interface )
	{
		ipc::memory::rpc< info_vector ( const module::name_vector& ) > registry_find( client, 1 );
		return registry_find( interface.names() );
	}

	void registry::insert( const module& module )
	{
		ipc::memory::rpc< void ( module::info_tuple, const module::name_vector& ) >
			registry_insert( client, 2 );
		registry_insert( module.info(), module.names() );
	}
}
