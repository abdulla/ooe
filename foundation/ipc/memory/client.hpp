/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_CLIENT_HPP
#define OOE_FOUNDATION_IPC_MEMORY_CLIENT_HPP

#include "foundation/ipc/memory/link.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

//--- client ---------------------------------------------------------------------------------------
class OOE_VISIBLE client
{
public:
    client( const local_address& );
    ~client( void );

    operator memory::transport&( void );

private:
    memory::transport transport;
    memory::link_client link;
};

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )

#endif  // OOE_FOUNDATION_IPC_MEMORY_CLIENT_HPP
