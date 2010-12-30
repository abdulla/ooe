/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_SERVER_PRIVATE_HPP
#define OOE_FOUNDATION_IPC_MEMORY_SERVER_PRIVATE_HPP

#include "foundation/utility/convert.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

void canary_handler( const void* address )
{
    throw error::runtime( "servlet: " ) << "Data violation at " << ptr( address );
}

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )

#endif  // OOE_FOUNDATION_IPC_MEMORY_SERVER_PRIVATE_HPP

