/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>
#include <map>

#include "component/registry/registry.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/ipc/semaphore.hpp"
#include "foundation/ipc/server.hpp"

namespace
{
	using namespace ooe;

	//--- typedefs -------------------------------------------------------------
	typedef tuple< std::string, module::type > module_tuple;
	typedef std::map< std::string, module::type > module_map;

	//--- globals --------------------------------------------------------------
	atom<> seed;
	std::string self;
	module_map modules;

	//--- rpc functions --------------------------------------------------------
	void insert( const std::string& path, module::type type )
	{
		modules.insert( module_map::value_type( path, type ) );
	}

	module_tuple find( const std::string& name, module::type type )
	{
		module_map::const_iterator i = modules.find( name );

		if ( i == modules.end() )
			throw error::runtime( "registry: " ) << "Unable to find module \"" << name << '\"';
		else if ( type == module::either || type == i->second )
			return i->second != module::either ? module_tuple( *i ) :
				module_tuple( i->first, module::internal );	// prefer internal loading
		else if ( type != module::external || i->second != module::either )
			throw error::runtime( "registry: " ) <<
				"Unable to find module \"" << name << "\" with type " << type;

		std::string path;
		path << "/ooe.surrogate." << hex( getpid() ) << '.' << hex( seed++ );

		{
			std::string gate = path + ".g";
			ipc::semaphore semaphore( gate.c_str(), ipc::semaphore::create, 0 );
			executable::spawn( self, "-p", path.c_str(), "-m", i->first.c_str(), 0 );
			semaphore.down();
		}

		return module_tuple( path, module::external );
	}

	//--- registry -------------------------------------------------------------
	void registry( const std::string& root, const std::string& name )
	{
		self = root + name;

		ipc::switchboard switchboard;
		switchboard.insert( insert );
		switchboard.insert( find );

		ipc::server server( ipc::create_semaphore, "/ooe.registry", switchboard );

		while ( !executable::signal() )
			server.decode();
	}

	//--- surrogate ------------------------------------------------------------
	void surrogate( const std::string& path, const std::string& module_name )
	{
		ipc::switchboard switchboard;
		ipc::server server( ipc::create_semaphore, path, switchboard );

		{
			std::string gate = path + ".g";
			ipc::semaphore semaphore( gate.c_str() );
			semaphore.up();
		}

		ooe::registry registry;
		module_type module = registry.find( module_name, module::internal );
		
		// iterate through interfaces, find facade for each, insert in to switchboard

		while ( !executable::signal() && server.decode() ) {}
	}

	//--- launch ---------------------------------------------------------------
	bool launch( const std::string& root, const std::string& name, s32 argc, c8** argv )
	{
		const c8* path = 0;
		const c8* module = 0;

		for ( s32 option; ( option = getopt( argc, argv, "p:m:" ) ) != -1; )
		{
			switch ( option )
			{
			case 'p':
				path = optarg;
				break;

			case 'm':
				module = optarg;
				break;

			default:
				return false;
			}
		}

		if ( path && module )
			surrogate( path, module );
		else
			registry( root, name );

		return true;
	}
}

//--- main ---------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
	return executable::launch( launch, argc, argv );
}
