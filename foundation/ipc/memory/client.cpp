/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/memory/client.hpp"
#include "foundation/ipc/memory/client_private.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe )( ipc )( memory ) )

//--- ipc_connect ----------------------------------------------------------------------------------
socket ipc_connect( const local_address& address, transport& transport )
{
    connect connect( address );
    transport.send( connect );
    return connect;
}

OOE_ANONYMOUS_END( ( ooe )( ipc )( memory ) )

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

//--- client ---------------------------------------------------------------------------------------
client::client( const local_address& address )
    : transport( ipc::unique_name() ), link( ipc_connect( address, transport ), transport )
{
    platform::ipc::memory::client_construct( transport );
}

client::~client( void )
{
}

client::operator memory::transport&( void )
{
    return transport;
}

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )
