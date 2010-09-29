/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <unistd.h>

#include "foundation/executable/program.hpp"

namespace ooe
{
//--- executable ---------------------------------------------------------------
    bool executable::path( c8* buffer, up_t size )
    {
        sp_t length = readlink( "/proc/self/exe", buffer, size - 1 );

        if ( length == -1 )
            return false;

        buffer[ length ] = 0;
        return true;
    }

//--- platform -----------------------------------------------------------------
    bool platform::launch( executable::launch_type launch,
        const std::string& root, const std::string& name, s32 argc, c8** argv )
    {
        return launch( root, name, argc, argv );
    }
}
