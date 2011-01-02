/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/program.hpp"
#include "foundation/ipc/memory/transport.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

void call_wait( const void* )
{
}

bool launch( const std::string&, const std::string&, s32, c8** )
{
    ipc::memory::transport transport( "/ooe", ipc::memory::transport::create );

    while ( !executable::has_signal() )
        transport.wait( call_wait, 0 );

    return true;
}

OOE_ANONYMOUS_END( ( ooe ) )

//--- main -----------------------------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
    return executable::launch( launch, argc, argv );
}
