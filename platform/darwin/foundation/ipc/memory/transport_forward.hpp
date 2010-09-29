/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_PLATFORM_DARWIN_FOUNDATION_IPC_MEMORY_TRANSPORT_FORWARD_HPP
#define OOE_PLATFORM_DARWIN_FOUNDATION_IPC_MEMORY_TRANSPORT_FORWARD_HPP

#include "foundation/ipc/semaphore.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

struct socket;

OOE_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( platform )( ipc )( memory ) )

class transport
{
protected:
    ooe::ipc::semaphore in;
    ooe::ipc::semaphore out;

    transport( const std::string&, bool );
    transport( ooe::socket& );
};

OOE_NAMESPACE_END( ( ooe )( platform )( ipc )( memory ) )

#endif  // OOE_PLATFORM_DARWIN_FOUNDATION_IPC_MEMORY_TRANSPORT_FORWARD_HPP
