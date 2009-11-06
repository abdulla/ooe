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

	void registry::insert( const module::info_tuple& info )
	{
		ipc::memory::rpc< void ( module::info_tuple ) > registry_insert( client, 2 );
		registry_insert( info );
	}

	void registry::scan( const std::string& path )
	{
		ipc::memory::rpc< void ( const std::string& ) > registry_scan( client, 3 );
		registry_scan( path );
	}

	std::string registry::surrogate( const std::string& path )
	{
		ipc::memory::rpc< std::string ( const std::string& ) > registry_surrogate( client, 4 );
		return registry_surrogate( path );
	}
}
