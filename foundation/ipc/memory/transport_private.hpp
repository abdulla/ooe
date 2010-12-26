/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_PRIVATE_HPP
#define OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_PRIVATE_HPP

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

enum lock_type
{
    unlocked,
    locked,
    sleeping
};

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )

#endif  // OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_PRIVATE_HPP
