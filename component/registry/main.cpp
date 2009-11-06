/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>
#include <map>

#include "component/registry/registry.hpp"
#include "foundation/executable/fork_io.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/io/directory.hpp"
#include "foundation/ipc/semaphore.hpp"
#include "foundation/ipc/memory/rpc.hpp"
#include "foundation/ipc/memory/server.hpp"
#include "foundation/parallel/lock.hpp"

namespace
{
	using namespace ooe;

	//--- globals --------------------------------------------------------------
	typedef std::multimap< module::name_tuple, module::info_tuple > module_map;
	std::string self;
	module_map modules;
	read_write mutex;

	//--- load -----------------------------------------------------------------
	module::name_vector load_library( const std::string& path )
	{
		library library( path );
		ooe::module module = library.find< ooe::module ( void ) >( "module_open" )();
		return module.names();
	}

	module::name_vector load_server( const std::string& path )
	{
		ipc::memory::client client( path );
		return ipc::memory::list( client )();
	}

	//--- ipc ------------------------------------------------------------------
	void insert( const module::info_tuple& info )
	{
		typedef module::name_vector::const_iterator iterator_type;
		module::name_vector names;

		switch ( info._0 )
		{
		case module::library:
			names = load_library( info._1 );
			break;

		case module::server:
			names = load_server( info._1 );
			break;

		default:
			throw error::runtime( "module: " ) << "Unknown module type: " << info._0;
		}

		for ( iterator_type i = names.begin(), end = names.end(); i != end; ++i )
		{
			write_lock lock( mutex );
			modules.insert( module_map::value_type( *i, info ) );
		}
	}

	registry::info_vector find( const module::name_vector& names )
	{
		typedef module::name_vector::const_iterator name_iterator;
		typedef std::map< module::info_tuple, up_t > histogram_map;
		histogram_map histogram;
		histogram_map::iterator k;

		for ( name_iterator i = names.begin(), end = names.end(); i != end; ++i )
		{
			{
				read_lock lock( mutex );
				module_map::const_iterator j = modules.find( *i );

				if ( j == modules.end() )
					continue;

				k = histogram.find( j->second );
			}

			if ( k == histogram.end() )
				histogram.insert( histogram_map::value_type( k->first, 1 ) );
			else
				++k->second;
		}

		typedef histogram_map::const_iterator histogram_iterator;
		registry::info_vector vector;

		for ( histogram_iterator i = histogram.begin(), end = histogram.end(); i != end; ++i )
		{
			if ( i->second == names.size() )
				vector.push_back( i->first );
		}

		return vector;
	}

	void scan( const std::string& path )
	{
		directory library( path );

		while ( ++library )
			insert( module::info_tuple( module::library, *library ) );
	}

	std::string surrogate( const std::string& path )
	{
		std::string name = ipc::unique_name();
		ipc::semaphore semaphore( name, ipc::semaphore::create, 0 );
		fork_io fork;

		if ( fork.is_child() )
			fork_io::execute( self, "-m", path.c_str(), "-s", name.c_str(), 0 );

		semaphore.down();
		return name;
	}

	//--- registry -------------------------------------------------------------
	void registry( const std::string& root, const std::string& name )
	{
		self = root + name;
		ipc::memory::switchboard switchboard;

		if ( switchboard.insert( find ) != 1 )
			throw error::runtime( "registry: " ) << "\"find\" not at index 1";
		else if ( switchboard.insert( insert ) != 2 )
			throw error::runtime( "registry: " ) << "\"insert\" not at index 2";
		else if ( switchboard.insert( scan ) != 3 )
			throw error::runtime( "registry: " ) << "\"scan\" not at index 3";
		else if ( switchboard.insert( surrogate ) != 4 )
			throw error::runtime( "registry: " ) << "\"surrogate\" not at index 4";

		ipc::memory::server server( "/ooe.registry", switchboard );

		while ( !executable::signal() )
			server.decode();
	}

	//--- surrogate ------------------------------------------------------------
	void surrogate( const std::string& path, const std::string& /*module_name*/ )
	{
		ipc::memory::switchboard switchboard;
		ipc::memory::server server( path, switchboard );
		ipc::semaphore( path ).up();

		while ( !executable::signal() && server.decode() ) {}
	}

	//--- launch ---------------------------------------------------------------
	bool launch( const std::string& root, const std::string& name, s32 argc, c8** argv )
	{
		const c8* module_path = 0;
		const c8* surrogate_path = 0;

		for ( s32 option; ( option = getopt( argc, argv, "m:s:u:" ) ) != -1; )
		{
			switch ( option )
			{
			case 'm':
				module_path = optarg;
				break;

			case 's':
				surrogate_path = optarg;
				break;

			case 'u':
				{ ipc::semaphore( optarg ).up(); }
				break;

			default:
				return false;
			}
		}

		if ( module_path && surrogate_path )
			surrogate( surrogate_path, module_path );
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
