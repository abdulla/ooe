/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/executable/program.hpp"
#include "foundation/executable/timer.hpp"
#include "foundation/parallel/thread_pool.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

bool launch( const std::string&, const std::string&, s32, c8** )
{
    thread_pool pool( "pool" );
    timer timer;

    for ( s32 i = 0; i != 10000; ++i )
        async( pool, function< void ( void ) >() );

    f32 elapsed = timer.elapsed();
    std::cout << elapsed << '\n';
    return true;
}

OOE_ANONYMOUS_END( ( ooe ) )

//--- main -----------------------------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
    return executable::launch( launch, argc, argv );
}
