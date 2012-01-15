/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <iostream>
#include <map>
#include <set>

#include "component/registry/module.hpp"
#include "component/registry/registry.hpp"
#include "foundation/executable/fork_io.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/ipc/semaphore.hpp"
#include "foundation/ipc/switchboard.hpp"
#include "foundation/ipc/memory/rpc.hpp"
#include "foundation/ipc/memory/server.hpp"
#include "foundation/parallel/lock.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

typedef tuple< std::string, std::string > name_tuple;
typedef tuple< registry::type, std::string > info_tuple;
typedef std::multimap< name_tuple, info_tuple > module_map;
typedef std::set< info_tuple > module_set;
std::string surrogate_path;
module_map map;
module_set set;
mutex mutex;
read_write read_write;

//--- list_all -------------------------------------------------------------------------------------
registry::info_vector list_all( void )
{
    lock lock( mutex );
    return registry::info_vector( set.begin(), set.end() );
}

//--- surrogate ------------------------------------------------------------------------------------
std::string surrogate( const std::string& path, bool public_server )
{
    std::string name = ipc::unique_name();
    ipc::barrier_wait wait( name + ".b" );
    fork_io fork;

    if ( fork.is_child() )
    {
        const c8* flag = public_server ? "-p" : 0;
        fork_io::execute( surrogate_path, "-l", path.c_str(), "-s", name.c_str(), flag, NULL );
    }

    return name;
}

//--- load_server ----------------------------------------------------------------------------------
interface::vector_type load_server( const std::string& path )
{
    ipc::memory::client client( ipc::local_name( path ) );
    return ipc::memory::list( client )();
}

//--- load_library ---------------------------------------------------------------------------------
interface::vector_type load_library( const std::string& path )
{
    /*library library( path );
    ooe::module module = library.find< ooe::module ( void ) >( "module_open" )();
    return module.names();*/

    // for security, load library as a surrogate
    return load_server( surrogate( path, false ) );
}

//--- insert ---------------------------------------------------------------------------------------
void insert( registry::type type, const std::string& path )
{
    info_tuple info( type, path );

    {
        lock lock( mutex );

        if ( !set.insert( info ).second )
            throw error::runtime( "registry: " ) << "Module " << info << " exists";
    }

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
    write_lock lock( read_write );

    for ( iterator_type i = vector.begin(), end = vector.end(); i != end; ++i )
        map.insert( std::make_pair( *i, info ) );
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
        read_lock lock( read_write );

        for ( pair_type j = map.equal_range( *i ); j.first != j.second; ++j.first )
        {
            std::pair< histogram_map::iterator, bool > pair =
                histogram.insert( std::make_pair( j.first->second, 1 ) );

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

//--- print ----------------------------------------------------------------------------------------
void print( const registry::info_vector& iv )
{
    for ( registry::info_vector::const_iterator i = iv.begin(), end = iv.end(); i != end; ++i )
    {
        switch ( i->_0 )
        {
        case registry::server:
            std::cout << "Server ";
            break;

        case registry::library:
            std::cout << "Library";
            break;

        default:
            std::cout << "Unknown";
            break;
        };

        std::cout << " -> " << i->_1 << '\n';
    }
}

//--- launch ---------------------------------------------------------------------------------------
bool launch( const std::string& root, const std::string&, s32 argc, c8** argv )
{
    c8* up_name = 0;

    for ( s32 option; ( option = getopt( argc, argv, "al:s:u:" ) ) != -1; )
    {
        switch ( option )
        {
        case 'a':
            print( registry().list_all() );
            return true;

        case 'l':
            registry().insert( registry::library, optarg );
            return true;

        case 's':
            registry().insert( registry::server, optarg );
            return true;

        case 'u':
            up_name = optarg;
            break;

        default:
            std::cout <<
                "    -a         List all known modules\n"
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
    else if ( switchboard.insert( list_all ) != 4 )
        throw error::runtime( "registry: " ) << "\"list_all\" not at index 4";

    ipc::memory::server server( switchboard );
    listen listen( ipc::server_address( "ooe.registry" ) );

    if ( up_name )
        ipc::barrier_notify( up_name );

    while ( !executable::has_signal() )
        server.insert( listen.accept() );

    return true;
}

OOE_ANONYMOUS_END( ( ooe ) )

//--- main -----------------------------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
    return executable::launch( launch, argc, argv );
}
