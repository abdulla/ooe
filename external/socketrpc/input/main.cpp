/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "external/socketrpc/utility.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/executable/timer.hpp"
#include "foundation/ipc/socket/client.hpp"
#include "foundation/ipc/socket/rpc.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

bool launch( const std::string&, const std::string&, s32 argc, c8** argv )
{
    up_t size = parse( argc, argv );
    std::string data( size, '.' );

    ipc::socket::client client( local_address( ipc::local_name( "ooe" ) ) );
    ipc::socket::find find( client );
    ipc::socket::rpc< void ( const std::string& ) >
        call( client, find( "call_input", typeid( void ( const c8* ) ).name() )() );

    timer timer;

    for ( up_t i = 1; i != iteration_limit; ++i )
        call( data );

    call( data )();
    f32 elapsed = timer.elapsed();

    std::cout << elapsed * microsecond_multiply << '\n';
    return true;
}

OOE_ANONYMOUS_END( ( ooe ) )

//--- main -----------------------------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
    return executable::launch( launch, argc, argv );
}
