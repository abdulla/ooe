/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <cerrno>

#include <sys/socket.h>

#include "foundation/io/error.hpp"
#include "foundation/io/socket.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

//--- socket ---------------------------------------------------------------------------------------
socket::socket( const descriptor& fd )
    : descriptor( fd )
{
    u32 value = 1;

    if ( setsockopt( get(), SOL_SOCKET, SO_NOSIGPIPE, &value, sizeof( value ) ) )
        throw error::io( "socket: " ) << "Unable to set option: " << error::number( errno );
}

OOE_NAMESPACE_END( ( ooe )( platform ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- socket ---------------------------------------------------------------------------------------
socket::peer_tuple socket::peer_id( void ) const
{
    uid_t uid;
    gid_t gid;

    if ( getpeereid( get(), &uid, &gid ) )
        throw error::io( "socket: " ) << "Unable to get peer id: " << error::number( errno );

    return make_tuple( uid, gid );
}

OOE_NAMESPACE_END( ( ooe ) )
