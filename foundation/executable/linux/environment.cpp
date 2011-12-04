/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <boost/lexical_cast.hpp>

#include "foundation/executable/environment.hpp"
#include "foundation/utility/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( executable ) )

up_t cache_line_size( void )
{
    const c8 sys_path[] = "/sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size";
    s32 fd = open( sys_path, O_RDONLY );

    if ( fd == -1 )
        throw error::runtime( "executable: " ) <<
            "Unable to open \"" << sys_path << "\": " << error::number( errno );

    c8 buffer[ 32 ];
    sp_t result = read( fd, buffer, sizeof( buffer ) );

    if ( result == -1 )
        throw error::runtime( "executable: " ) <<
            "Unable to read \"" << sys_path << "\": " << error::number( errno );

    buffer[ result ] = 0;
    return boost::lexical_cast< up_t >( buffer );
}

OOE_NAMESPACE_END( ( ooe )( executable ) )
