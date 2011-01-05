/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/io/directory.hpp"
#include "foundation/ipc/name.hpp"
#include "foundation/ipc/memory/client.hpp"
#include "foundation/ipc/memory/rpc.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe )( ipc )( memory ) )

//--- ipc_connect ----------------------------------------------------------------------------------
link_t ipc_connect( const std::string& server_name, const std::string& client_name )
{
    connect connect( local_address( local_name( server_name ) ) );
    transport transport( connect );

    rpc< link_t ( const std::string& ) > link( transport, 1 );
    return link( client_name );
}

//--- ipc_disconnect -------------------------------------------------------------------------------
void ipc_disconnect( const std::string& server_name, link_t link )
{
    connect connect( local_address( local_name( server_name ) ) );
    transport transport( connect );

    rpc< void ( link_t ) > unlink( transport, 2 );
    unlink( link );
}

//--- ipc_socket -----------------------------------------------------------------------------------
socket ipc_socket( const std::string& server_name, link_t& link, transport& transport )
{
    std::string client_name = transport.name();
    std::string name = local_name( client_name );

    if ( exists( name ) )
        erase( name );

    listen listen( ( local_address( name ) ) );
    link = ipc_connect( server_name, client_name );
    socket socket = listen.accept();
    erase( name );

    transport.send( socket );
    transport.unlink();
    return socket;
}

OOE_ANONYMOUS_END( ( ooe )( ipc )( memory ) )

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

//--- client ---------------------------------------------------------------------------------------
client::client( const std::string& name_ )
    : name( name_ ), link(), transport( ipc::unique_name() ),
    link_client( ipc_socket( name, link, transport ), transport )
{
}

client::~client( void )
{
    if ( !link_client )
        return;

    link_client.shutdown();
    OOE_PRINT( "ipc::memory::client", ipc_disconnect( name, link ) );
}

client::operator memory::transport&( void )
{
    return transport;
}

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )
