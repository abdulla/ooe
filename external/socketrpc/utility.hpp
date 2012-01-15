/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_EXTERNAL_SOCKETRPC_UTILITY_HPP
#define OOE_EXTERNAL_SOCKETRPC_UTILITY_HPP

#include <boost/lexical_cast.hpp>

#include "foundation/utility/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

const up_t iteration_limit = 10000;
const f32 microsecond_multiply = 1000000. / iteration_limit;

inline up_t parse( s32 argc, c8** argv )
{
    if ( argc < 2 )
        throw error::runtime( "parse: " ) << "Argument required, none given";

    up_t value = boost::lexical_cast< up_t >( argv[ 1 ] );

    if ( !value )
        throw error::runtime( "parse: " ) << "Invalid argument, integer > 0 required";

    return value - 1;
}

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_EXTERNAL_SOCKETRPC_UTILITY_HPP
