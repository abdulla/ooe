/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_DARWIN_CLIENT_PRIVATE_HPP
#define OOE_FOUNDATION_IPC_MEMORY_DARWIN_CLIENT_PRIVATE_HPP

#include "foundation/ipc/memory/rpc.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( platform )( ipc )( memory ) )

void client_construct( ooe::ipc::memory::transport& transport )
{
    ooe::ipc::memory::rpc< void ( void ) > null( transport, 0 );
    null();
}

OOE_NAMESPACE_END( ( ooe )( platform )( ipc )( memory ) )

#endif  // OOE_FOUNDATION_IPC_MEMORY_DARWIN_CLIENT_PRIVATE_HPP
