/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <sys/socket.h>

#include "foundation/io/socket.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

//--- socket ---------------------------------------------------------------------------------------
socket::socket( const descriptor& fd )
    : descriptor( fd )
{
    static_cast< ooe::socket* >( this )->option( SO_NOSIGPIPE, true );
}

OOE_NAMESPACE_END( ( ooe )( platform ) )
