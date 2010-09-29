/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>
#include <map>
#include <set>

#include "component/registry/module.hpp"
#include "component/registry/registry.hpp"
#include "foundation/executable/fork_io.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/ipc/switchboard.hpp"
#include "foundation/ipc/memory/rpc.hpp"
#include "foundation/ipc/memory/server.hpp"
#include "foundation/parallel/lock.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

typedef tuple< std::string, std::string > name_tuple;
typedef tuple< registry::type, std::string > info_tuple;
typedef std::multimap< name_tuple, info_tuple > module_map;
typedef std::set< info_tuple > module_set;
std::string surrogate_path;
module_map map;
module_set set;
read_write mutex;

interface::vector_type load_server( const std::string& );

//--- surrogate ------------------------------------------------------------------------------------
std::string surrogate( const std::string& path )
{
    std::string name = ipc::unique_name();
    ipc::barrier_wait wait( name + ".b" );
    fork_io fork;

    if ( fork.is_child() )
        fork_io::execute( surrogate_path, "-l", path.c_str(), "-s", name.c_str(), NULL );

    return name;
}

//--- load_library ---------------------------------------------------------------------------------
interface::vector_type load_library( const std::string& path )
{
    /*library library( path );
    ooe::module module = library.find< ooe::module ( void ) >( "module_open" )();
    return module.names();*/

    // for security, load library as a surrogate
    return load_server( surrogate( path ) );
}

//--- load_server ----------------------------------------------------------------------------------
interface::vector_type load_server( const std::string& path )
{
    ipc::memory::client client( path );
    return ipc::memory::list( client )();
}

//--- insert ---------------------------------------------------------------------------------------
void insert( registry::type type, const std::string& path )
{
    info_tuple info( type, path );

    if ( !set.insert( info ).second )
        throw error::runtime( "registry: " ) << "Module " << info << " exists";

    interface::vector_type vector;

    switch ( type )
    {
    case registry::library:
        vector = load_library( path );
        break;

    case registry::server:
        vector = load_server( path );
        break;

    default:
        throw error::runtime( "registry: " ) << "Unknown module type: " << type;
    }

    typedef interface::vector_type::const_iterator iterator_type;

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
            std::pair< histogram_map::iterator, bool > pair =
                histogram.insert( histogram_map::value_type( j.first->second, 1 ) );

            if ( !pair.second )
                ++pair.first->second;
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

//--- launch ---------------------------------------------------------------------------------------
bool launch( const std::string& root, const std::string&, s32 argc, c8** argv )
{
    const c8* up_name = 0;

    for ( s32 option; ( option = getopt( argc, argv, "l:s:u:" ) ) != -1; )
    {
        switch ( option )
        {
        case 'l':
            ooe::registry().insert( registry::library, optarg );
            return true;

        case 's':
            ooe::registry().insert( registry::server, optarg );
            return true;

        case 'u':
            up_name = optarg;
            break;

        default:
            std::cout <<
                "    -l <path>  Path of library module to insert in to registry\n"
                "    -s <path>  Path of server module to insert in to registry\n"
                "    -u <path>  Path of semaphore to increment\n";

            return false;
        }
    }

    surrogate_path = root + "surrogate";
    ipc::switchboard switchboard;

    if ( switchboard.insert( find ) != 1 )
        throw error::runtime( "registry: " ) << "\"find\" not at index 1";
    else if ( switchboard.insert( insert ) != 2 )
        throw error::runtime( "registry: " ) << "\"insert\" not at index 2";
    else if ( switchboard.insert( surrogate ) != 3 )
        throw error::runtime( "registry: " ) << "\"surrogate\" not at index 3";

    ipc::memory::server server( "/ooe.registry", switchboard );

    if ( up_name )
        ipc::barrier_notify( up_name );

    while ( !executable::has_signal() )
        server.decode();

    return true;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

//--- main -----------------------------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
    return executable::launch( launch, argc, argv );
}
