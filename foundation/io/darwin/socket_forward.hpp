/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IO_DARWIN_SOCKET_FORWARD_HPP
#define OOE_FOUNDATION_IO_DARWIN_SOCKET_FORWARD_HPP

#include "foundation/io/descriptor.hpp"

#define OOE_SOCKET_SEND_FLAG 0

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

class socket
    : public ooe::descriptor
{
protected:
    socket( const descriptor& );
};

OOE_NAMESPACE_END( ( ooe )( platform ) )

#endif  // OOE_FOUNDATION_IO_DARWIN_SOCKET_FORWARD_HPP
