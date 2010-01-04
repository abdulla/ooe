/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

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
		ipc::memory::rpc< info_vector ( const interface::vector_type& ) >
			registry_find( client, 1 );
		return registry_find( interface.get() );
	}

	void registry::insert( type flag, const std::string& path )
	{
		ipc::memory::rpc< void ( type, const std::string& ) > registry_insert( client, 2 );
		registry_insert( flag, path );
	}

	std::string registry::surrogate( const std::string& path )
	{
		ipc::memory::rpc< std::string ( const std::string& ) > registry_surrogate( client, 3 );
		return registry_surrogate( path );
	}
}
