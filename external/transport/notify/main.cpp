/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "external/memoryrpc/utility.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/executable/timer.hpp"
#include "foundation/ipc/name.hpp"
#include "foundation/ipc/memory/transport.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

bool launch( const std::string&, const std::string&, s32, c8** )
{
    connect connect( local_address( ipc::local_name( "ooe-transport" ) ) );
    ipc::memory::transport transport( connect );
    timer timer;

    for ( up_t i = 0; i != iteration_limit; ++i )
        transport.notify();

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
