/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "component/registry/local.hpp"
#include "component/registry/module.hpp"
#include "component/registry/registry.hpp"
#include "component/registry/remote.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/io/directory.hpp"
#include "foundation/io/poll.hpp"
#include "foundation/ipc/nameservice.hpp"
#include "foundation/ipc/semaphore.hpp"
#include "foundation/ipc/memory/server.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

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

//--- registry_insert ------------------------------------------------------------------------------
void registry_insert( void* surrogate_path )
{
    registry().insert( registry::server, static_cast< const c8* >( surrogate_path ) );
}

//--- poll_function --------------------------------------------------------------------------------
void poll_function( bool& state, const descriptor&, poll::type type )
{
    state = type == poll::input;
}

//--- public_server --------------------------------------------------------------------------------
void public_server( const c8* surrogate_path, ipc::memory::server& server, listen& listen )
{
    thread( "insert", registry_insert, const_cast< c8* >( surrogate_path ) );

    while ( !executable::has_signal() )
        server.insert( listen.accept() );
}

//--- private_server -------------------------------------------------------------------------------
void private_server( const c8* surrogate_path, ipc::memory::server& server, listen& listen )
{
    socket socket = listen.accept();
    erase( ipc::local_name( surrogate_path ) );
    server.insert( socket );

    bool state = true;
    ooe::poll poll;
    poll.insert( socket, poll::function_type( state, poll_function ) );

    while ( state )
        poll.wait();
}

//--- launch ---------------------------------------------------------------------------------------
bool launch( const std::string&, const std::string&, s32 argc, c8** argv )
{
    bool is_public = false;
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
            is_public = true;
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
    {
        std::cout << "The library and surrogate paths must be specified\n";
        return false;
    }

    library library( library_path );
    ipc::nameservice nameservice;
    load_nameservice( nameservice, library.find< ooe::module ( void ) >( "module_open" )() );

    ipc::memory::server server( nameservice );
    listen listen( ipc::server_address( surrogate_path ) );
    ipc::barrier_notify( std::string( surrogate_path ) + ".b" );

    if ( is_public )
        public_server( surrogate_path, server, listen );
    else
        private_server( surrogate_path, server, listen );

    return true;
}

OOE_ANONYMOUS_END( ( ooe ) )

//--- main -----------------------------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
    return executable::launch( launch, argc, argv );
}
