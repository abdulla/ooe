/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_PLATFORM_POSIX_FOUNDATION_IPC_MEMORY_TRANSPORT_FORWARD_HPP
#define OOE_PLATFORM_POSIX_FOUNDATION_IPC_MEMORY_TRANSPORT_FORWARD_HPP

OOE_NAMESPACE_BEGIN( ( ooe ) )

class semaphore;

OOE_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( platform )( ipc )( memory ) )

class transport
{
protected:
    bool created;

    transport( bool );
};

OOE_NAMESPACE_END( ( ooe )( platform )( ipc )( memory ) )

#endif  // OOE_PLATFORM_POSIX_FOUNDATION_IPC_MEMORY_TRANSPORT_FORWARD_HPP
