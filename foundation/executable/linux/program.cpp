/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <unistd.h>

#include "foundation/executable/program.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- platform -------------------------------------------------------------------------------------
bool platform::launch( executable::launch_type launch,
    const std::string& root, const std::string& name, s32 argc, c8** argv )
{
    return launch( root, name, argc, argv );
}

//--- executable -----------------------------------------------------------------------------------
bool executable::path( c8* buffer, up_t size )
{
    sp_t length = readlink( "/proc/self/exe", buffer, size - 1 );

    if ( length == -1 )
        return false;

    buffer[ length ] = 0;
    return true;
}

OOE_NAMESPACE_END( ( ooe ) )
