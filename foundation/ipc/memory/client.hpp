/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_CLIENT_HPP
#define OOE_FOUNDATION_IPC_MEMORY_CLIENT_HPP

#include "foundation/ipc/memory/link.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

//--- client ---------------------------------------------------------------------------------------
class OOE_VISIBLE client
{
public:
    client( const local_address& );
    operator memory::transport&( void );

private:
    memory::transport transport;
    memory::link_client link_client;
};

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )

#endif  // OOE_FOUNDATION_IPC_MEMORY_CLIENT_HPP
