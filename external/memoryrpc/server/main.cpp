/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/program.hpp"
#include "foundation/ipc/nameservice.hpp"
#include "foundation/ipc/memory/server.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

std::string value;

void set_output( up_t size )
{
    value = std::string( size, '.' );
}

void call_noop( void )
{
}

const c8* call_output( void )
{
    return value.c_str();
}

void call_input( const c8* )
{
}

bool launch( const std::string&, const std::string&, s32, c8** )
{
    ipc::nameservice nameservice;
    nameservice.insert( "set_output", set_output );
    nameservice.insert( "call_noop", call_noop );
    nameservice.insert( "call_output", call_output );
    nameservice.insert( "call_input", call_input );

    ipc::memory::server server( nameservice );
    listen listen( ipc::server_address( "ooe" ) );

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
