/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_EXTERNAL_MEMORYRPC_UTILITY_HPP
#define OOE_EXTERNAL_MEMORYRPC_UTILITY_HPP

#include <cstdlib>

#include "foundation/utility/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

const up_t iteration_limit = 10000;
const f32 microsecond_multiply = 1000000. / iteration_limit;

inline up_t parse( s32 argc, c8** argv )
{
    if ( argc < 2 )
        throw error::runtime( "parse: " ) << "Argument required, none given";

    c8* end;
    up_t value = std::strtoul( argv[ 1 ], &end, 10 );

    if ( *end || !value )
        throw error::runtime( "parse: " ) << "Invalid argument, integer > 0 required";

    return value - 1;
}

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_EXTERNAL_MEMORYRPC_UTILITY_HPP
