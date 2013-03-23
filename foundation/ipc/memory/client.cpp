/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/memory/client.hpp"

#include OOE_PATH( foundation/ipc/memory, client_private.hpp )

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

//--- client ---------------------------------------------------------------------------------------
client::client( const local_address& address )
    : connect( address ), transport( ipc::unique_name() ), link( connect, transport )
{
    transport.send( connect );
    platform::ipc::memory::client_construct( transport );
}

client::~client( void )
{
}

client::operator ooe::socket&( void )
{
    return connect;
}

client::operator memory::transport&( void )
{
    return transport;
}

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )
