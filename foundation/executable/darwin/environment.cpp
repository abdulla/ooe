/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <sys/sysctl.h>

#include "foundation/executable/environment.hpp"
#include "foundation/utility/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( executable ) )

up_t cache_line_size( void )
{
    s32 name[] = { CTL_HW, HW_CACHELINE };
    up_t result;
    up_t size = sizeof( result );

    if ( sysctl( name, 2, &result, &size, 0, 0 ) )
        throw error::runtime( "executable: " ) << "Unable to query cache line size";

    return result;
}

OOE_NAMESPACE_END( ( ooe )( executable ) )
