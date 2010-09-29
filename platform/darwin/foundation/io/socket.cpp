/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <sys/socket.h>

#include "foundation/io/socket.hpp"

namespace ooe
{
//--- platform::socket ---------------------------------------------------------
    platform::socket::socket( const descriptor& fd )
        : descriptor( fd )
    {
        static_cast< ooe::socket* >( this )->option( SO_NOSIGPIPE, true );
    }
}
