/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>
#include <map>
#include <set>

#include "component/registry/local.hpp"
#include "component/registry/module.hpp"
#include "component/registry/registry.hpp"
#include "component/registry/remote.hpp"
#include "foundation/executable/checkpoint.hpp"
#include "foundation/executable/fork_io.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/ipc/nameservice.hpp"
#include "foundation/ipc/semaphore.hpp"
#include "foundation/ipc/memory/rpc.hpp"
#include "foundation/ipc/memory/server.hpp"
#include "foundation/parallel/lock.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

typedef tuple< std::string, std::string > name_tuple;
typedef tuple< registry::type, std::string > info_tuple;
typedef std::multimap< name_tuple, info_tuple > module_map;
typedef std::set< info_tuple > module_set;
std::string self;
module_map map;
module_set set;
read_write mutex;

void insert( registry::type, const std::string& );
registry::info_vector find( const interface::vector_type& );
std::string surrogate( const std::string& );

//--- load_server ----------------------------------------------------------------------------------
module::vector_type load_server( const std::string& path )
{
	ipc::memory::client client( path );
	return ipc::memory::list( client )();
}

//--- load_library ---------------------------------------------------------------------------------
module::vector_type load_library( const std::string& path )
{
	/*library library( path );
	ooe::module module = library.find< ooe::module ( void ) >( "module_open" )();
	return module.get();*/

	// for security, load library as a surrogate
	return load_server( surrogate( path ) );
}

//--- load_nameservice -----------------------------------------------------------------------------
void load_nameservice( ipc::nameservice& nameservice, const module& module )
{
	const module::vector_type& names = module.get();
	const facade::remote::vector_type& remotes = static_cast< const facade::remote* >
		( module.find( typeid( facade::remote ).name() ) )->get();
	const facade::local::vector_type& locals = static_cast< const facade::local* >
		( module.find( typeid( facade::local ).name() ) )->get();
	up_t size = names.size();

	if ( remotes.size() != size || locals.size() != size )
		throw error::runtime( "surrogate: " ) << "Incorrect number of callbacks";

	for ( up_t i = 0; i != size; ++i )
		nameservice.insert_direct( names[ i ]._0, names[ i ]._1, remotes[ i ], locals[ i ] );
}

//--- insert ---------------------------------------------------------------------------------------
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

//--- find -----------------------------------------------------------------------------------------
registry::info_vector find( const interface::vector_type& names )
{
	typedef interface::vector_type::const_iterator name_iterator;
	typedef std::map< info_tuple, up_t > histogram_map;
	histogram_map histogram;

	for ( name_iterator i = names.begin(), end = names.end(); i != end; ++i )
	{
		typedef std::pair< module_map::const_iterator, module_map::const_iterator > pair_type;
		read_lock lock( mutex );

		for ( pair_type j = map.equal_range( *i ); j.first != j.second; ++j.first )
		{
			histogram_map::iterator k = histogram.find( j.first->second );

			if ( k == histogram.end() )
				histogram.insert( histogram_map::value_type( j.first->second, 1 ) );
			else
				++k->second;
		}
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

//--- surrogate ------------------------------------------------------------------------------------
std::string surrogate( const std::string& path )
{
	std::string name = ipc::unique_name();
	ipc::barrier_wait wait( name + ".g" );
	fork_io fork;

	if ( fork.is_child() )
		fork_io::execute( self, "-m", path.c_str(), "-s", name.c_str(), NULL );

	return name;
}

//--- surrogate_launch -----------------------------------------------------------------------------
void surrogate_launch( const std::string& surrogate_path, const std::string& module_path )
{
	library library( module_path );
	ipc::nameservice nameservice;
	load_nameservice( nameservice, library.find< ooe::module ( void ) >( "module_open" )() );

	ipc::memory::server server( surrogate_path, nameservice );
	ipc::barrier_notify( surrogate_path + ".g" );

	while ( !executable::signal() && server.decode() ) {}
}

//--- registry_launch ------------------------------------------------------------------------------
void registry_launch( const std::string& self_path, const c8* up_name )
{
	self = self_path;
	ipc::switchboard switchboard;

	if ( switchboard.insert( find ) != 1 )
		throw error::runtime( "registry: " ) << "\"find\" not at index 1";
	else if ( switchboard.insert( insert ) != 2 )
		throw error::runtime( "registry: " ) << "\"insert\" not at index 2";
	else if ( switchboard.insert( surrogate ) != 3 )
		throw error::runtime( "registry: " ) << "\"surrogate\" not at index 4";

	ipc::memory::server server( "/ooe.registry", switchboard );

	if ( up_name )
		ipc::barrier_notify( up_name );

	checkpoint checkpoint;

	while ( !executable::signal() )
	{
		if ( !server.decode() )
			checkpoint.update();
	}
}

//--- launch ---------------------------------------------------------------------------------------
bool launch( const std::string& root, const std::string& name, s32 argc, c8** argv )
{
	const c8* module_path = 0;
	const c8* surrogate_path = 0;
	const c8* up_name = 0;

	for ( s32 option; ( option = getopt( argc, argv, "l:m:r:s:u:" ) ) != -1; )
	{
		switch ( option )
		{
		case 'l':
			ooe::registry().insert( registry::library, optarg );
			return true;

		case 'm':
			module_path = optarg;
			break;

		case 'r':
			ooe::registry().insert( registry::server, optarg );
			return true;

		case 's':
			surrogate_path = optarg;
			break;

		case 'u':
			up_name = optarg;
			break;

		default:
			return false;
		}
	}

	if ( module_path && surrogate_path )
		surrogate_launch( surrogate_path, module_path );
	else
		registry_launch( root + name, up_name );

	return true;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

//--- main -----------------------------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
	return executable::launch( launch, argc, argv );
}
