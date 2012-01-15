/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/executable/program.hpp"
#include "foundation/ipc/nameservice.hpp"
#include "foundation/ipc/semaphore.hpp"
#include "foundation/ipc/memory/server.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

void hello( void )
{
    std::cout << "hello server\n";
}

bool launch( const std::string&, const std::string&, s32 argc, c8** argv )
{
    c8* up_name = 0;

    for ( s32 option; ( option = getopt( argc, argv, "u:" ) ) != -1; )
    {
        switch ( option )
        {
        case 'u':
            up_name = optarg;
            break;

        default:
            return false;
        }
    }

    ipc::nameservice nameservice;
    nameservice.insert( "hello", hello, "A function that prints 'hello server'" );

    ipc::memory::server server( nameservice );
    listen listen( ipc::server_address( "ooe.hello" ) );

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
