/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/io/socket.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

//--- socket ---------------------------------------------------------------------------------------
socket::socket( const descriptor& fd )
    : descriptor( fd )
{
}

OOE_NAMESPACE_END( ( ooe )( platform ) )
