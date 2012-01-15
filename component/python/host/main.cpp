/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "component/python/facade.hpp"
#include "component/python/vm.hpp"
#include "foundation/executable/program.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

bool launch( const std::string&, const std::string&, s32 argc, c8** argv )
{
    python::vm vm;
    vm.setup( python::component_setup );

    for ( s32 i = 1; i < argc; ++i )
    {
        const c8* name = std::strrchr( argv[ i ], '/' );
        name = name ? name + 1 : argv[ i ];

        vm.load( name, descriptor( argv[ i ] ) );
    }

    return true;
}

OOE_ANONYMOUS_END( ( ooe ) )

//--- main -----------------------------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
    return executable::launch( launch, argc, argv );
}
