/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>
#include <map>
#include <set>

#include "component/registry/local.hpp"
#include "component/registry/module.hpp"
#include "component/registry/registry.hpp"
#include "component/registry/remote.hpp"
#include "foundation/executable/fork_io.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/ipc/semaphore.hpp"
#include "foundation/ipc/memory/nameservice.hpp"
#include "foundation/ipc/memory/rpc.hpp"
#include "foundation/ipc/memory/server.hpp"
#include "foundation/parallel/lock.hpp"

namespace
{
	using namespace ooe;

	//--------------------------------------------------------------------------
	typedef tuple< std::string, std::string > name_tuple;
	typedef tuple< registry::type, std::string > info_tuple;
	typedef std::multimap< name_tuple, info_tuple > module_map;
	typedef std::set< info_tuple > module_set;
	std::string self;
	module_map map;
	module_set set;
	read_write mutex;

	module::vector_type load_server( const std::string& path )
	{
		ipc::memory::client client( path );
		return ipc::memory::list( client )();
	}

	module::vector_type load_library( const std::string& path )
	{
		library library( path );
		ooe::module module = library.find< ooe::module ( void ) >( "module_open" )();
		return module.get();
	}

	void load_nameservice( ipc::memory::nameservice& nameservice, const module& module )
	{
		const module::vector_type& names = module.get();
		const facade::remote::vector_type& remotes =
			static_cast< const facade::remote* >( module.find( "remote" ) )->get();
		const facade::local::vector_type& locals =
			static_cast< const facade::local* >( module.find( "local" ) )->get();
		up_t size = names.size();

		if ( remotes.size() != size || locals.size() != size )
			throw error::runtime( "surrogate: " ) << "Incorrect number of callbacks";

		for ( up_t i = 0; i != size; ++i )
			nameservice.insert_direct( names[ i ]._0, names[ i ]._1, remotes[ i ], locals[ i ] );
	}

	//--------------------------------------------------------------------------
	void insert( registry::type type, const std::string& path )
	{
		info_tuple info( type, path );

		if ( set.find( info ) != set.end() )
			throw error::runtime( "registry: " ) << "Module " << info << " exists";
		else
			set.insert( info );

		module::vector_type vector;

		switch ( type )
		{
		case registry::server:
			vector = load_server( path );
			break;

		case registry::library:
			vector = load_library( path );
			break;

		default:
			throw error::runtime( "registry: " ) << "Unknown module type: " << type;
		}

		typedef module::vector_type::const_iterator iterator_type;

		for ( iterator_type i = vector.begin(), end = vector.end(); i != end; ++i )
		{
			write_lock lock( mutex );
			map.insert( module_map::value_type( *i, info ) );
		}
	}

	registry::info_vector find( const interface::vector_type& names )
	{
		typedef interface::vector_type::const_iterator name_iterator;
		typedef std::map< info_tuple, up_t > histogram_map;
		histogram_map histogram;
		histogram_map::iterator k;

		for ( name_iterator i = names.begin(), end = names.end(); i != end; ++i )
		{
			{
				read_lock lock( mutex );
				module_map::const_iterator j = map.find( *i );

				if ( j == map.end() )
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

	std::string surrogate( const std::string& path )
	{
		std::string name = ipc::unique_name();
		ipc::semaphore semaphore( name + ".g", ipc::semaphore::create, 0 );
		fork_io fork;

		if ( fork.is_child() )
			fork_io::execute( self, "-m", path.c_str(), "-s", name.c_str(), 0 );

		semaphore.down();
		return name;
	}

	//--------------------------------------------------------------------------
	void registry( const std::string& root, const std::string& name )
	{
		self = root + name;
		ipc::memory::switchboard switchboard;

		if ( switchboard.insert( find ) != 1 )
			throw error::runtime( "registry: " ) << "\"find\" not at index 1";
		else if ( switchboard.insert( insert ) != 2 )
			throw error::runtime( "registry: " ) << "\"insert\" not at index 2";
		else if ( switchboard.insert( surrogate ) != 3 )
			throw error::runtime( "registry: " ) << "\"surrogate\" not at index 4";

		ipc::memory::server server( "/ooe.registry", switchboard );

		while ( !executable::signal() )
			server.decode();
	}

	void surrogate( const std::string& surrogate_path, const std::string& module_path )
	{
		library library( module_path );
		ipc::memory::nameservice nameservice;
		load_nameservice( nameservice, library.find< ooe::module ( void ) >( "module_open" )() );

		ipc::memory::server server( surrogate_path, nameservice );
		ipc::semaphore( surrogate_path + ".g" ).up();

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
