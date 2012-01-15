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
}

OOE_NAMESPACE_END( ( ooe )( platform ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- socket ---------------------------------------------------------------------------------------
socket::peer_tuple socket::peer_id( void ) const
{
    ucred credential;
    u32 length = sizeof( credential );

    if ( getsockopt( get(), SOL_SOCKET, SO_PEERCRED, &credential, &length ) )
        throw error::io( "socket: " ) << "Unable to get peer id: " << error::number( errno );

    return make_tuple( credential.uid, credential.gid );
}

OOE_NAMESPACE_END( ( ooe ) )
