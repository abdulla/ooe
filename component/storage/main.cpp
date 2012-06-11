/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/program.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

//--- launch ---------------------------------------------------------------------------------------
bool launch( const std::string&, const std::string&, s32, c8** )
{
    return true;
}

OOE_ANONYMOUS_END( ( ooe ) )

//--- main -----------------------------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
    return executable::launch( launch, argc, argv );
}
