/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/program.hpp"
#include "foundation/ipc/name.hpp"
#include "foundation/ipc/memory/client.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

bool launch( const std::string&, const std::string&, s32, c8** )
{
    ipc::memory::client client( ipc::local_name( "ooe" ) );
    return true;
}

OOE_ANONYMOUS_END( ( ooe ) )

//--- main -----------------------------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
    return executable::launch( launch, argc, argv );
}
