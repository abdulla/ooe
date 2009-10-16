/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "component/registry/registry.hpp"
#include "foundation/ipc/memory/rpc.hpp"

namespace
{
	using namespace ooe;

	void ooe_module_close( void* )
	{
	}

	any* ooe_module_find( void*, const std::string& interface )
	{
		throw error::runtime( "module_internal: " ) <<
			"Unable to find interface \"" << interface << '\"';
	}
}

namespace ooe
{
//--- module_internal ----------------------------------------------------------
	module_internal::module_internal( const std::string& path )
		: library( path ), builder()
	{
	}

	module::type module_internal::state( void ) const
	{
		return internal;
	}

	module::list_type module_internal::interface_list( void ) const
	{
		return list_type();
	}

	any* module_internal::find( const std::string& ) const
	{
		return 0;
	}

//--- module_external ----------------------------------------------------------
	module_external::module_external( const std::string& path )
		: client( path )
	{
	}

	module::type module_external::state( void ) const
	{
		return external;
	}

	u32 module_external::find( const std::string& interface ) const
	{
		typedef ipc::memory::rpc< u32 ( const std::string& ) > rpc_type;
		return rpc_type( client, 1 )( interface );
	}

	module::list_type module_external::interface_list( void ) const
	{
		return list_type();
	}

	ipc::memory::client& module_external::get( void ) const
	{
		return client;
	}

//--- registry -----------------------------------------------------------------
	registry::registry( void )
		: client( "/ooe.registry" )
	{
	}

	void registry::insert( const std::string& path, module::type type )
	{
		typedef ipc::memory::rpc< void ( const std::string&, module::type ) > rpc_type;
		rpc_type( client, 1 )( path, type );
	}

	module_type registry::find( const std::string& name, module::type type ) const
	{
		typedef tuple< std::string, module::type > tuple_type;
		typedef ipc::memory::rpc< tuple_type ( const std::string&, module::type ) > rpc_type;
		tuple_type tuple = rpc_type( client, 2 )( name, type );

		return tuple._1 == module::internal ?
			module_type( new module_internal( tuple._0 ) ) :
			module_type( new module_external( tuple._0 ) );
	}
}

//--- ooe_module_open --------------------------------------------------
extern "C" module_tuple ooe_module_open( void )
{
	return module_tuple( 0, ooe_module_close, ooe_module_find );
}
