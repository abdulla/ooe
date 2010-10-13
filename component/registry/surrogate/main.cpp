/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "component/registry/local.hpp"
#include "component/registry/module.hpp"
#include "component/registry/registry.hpp"
#include "component/registry/remote.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/ipc/nameservice.hpp"
#include "foundation/ipc/memory/server.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

//--- load_nameservice -----------------------------------------------------------------------------
void load_nameservice( ipc::nameservice& nameservice, const module& module )
{
    const interface::vector_type& names = module.names();
    const module::vector_type& docs = module.docs();
    const facade::remote::vector_type& remotes = static_cast< const facade::remote* >
        ( module.find( typeid( facade::remote ).name() ) )->get();
    const facade::local::vector_type& locals = static_cast< const facade::local* >
        ( module.find( typeid( facade::local ).name() ) )->get();
    up_t size = names.size();

    if ( remotes.size() != size || locals.size() != size )
        throw error::runtime( "surrogate: " ) << "Incorrect number of callbacks";

    for ( up_t i = 0; i != size; ++i )
        nameservice.insert_direct
            ( names[ i ]._0, names[ i ]._1, docs[ i ], remotes[ i ], locals[ i ] );
}

void* registry_insert( void* surrogate_path )
{
    registry().insert( registry::server, static_cast< const c8* >( surrogate_path ) );
    return 0;
}

//--- launch ---------------------------------------------------------------------------------------
bool launch( const std::string&, const std::string&, s32 argc, c8** argv )
{
    bool public_server = false;
    const c8* library_path = 0;
    const c8* surrogate_path = 0;

    for ( s32 option; ( option = getopt( argc, argv, "l:ps:" ) ) != -1; )
    {
        switch ( option )
        {
        case 'l':
            library_path = optarg;
            break;

        case 'p':
            public_server = true;
            break;

        case 's':
            surrogate_path = optarg;
            break;

        default:
            std::cout <<
                "    -p         Create a public server from surrogate\n"
                "    -l <path>  Path of library module to load in to surrogate\n"
                "    -s <path>  Path to use for surrogate\n";

            return false;
        }
    }

    if ( !library_path || !surrogate_path )
        return false;

    library library( library_path );
    ipc::nameservice nameservice;
    load_nameservice( nameservice, library.find< ooe::module ( void ) >( "module_open" )() );

    ipc::memory::server server( surrogate_path, nameservice );
    ipc::barrier_notify( std::string( surrogate_path ) + ".b" );

    if ( public_server )
        thread( registry_insert, const_cast< c8* >( surrogate_path ) );

    while ( !executable::has_signal() && ( public_server || server.decode() ) ) {}

    return true;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

//--- main -----------------------------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
    return executable::launch( launch, argc, argv );
}
