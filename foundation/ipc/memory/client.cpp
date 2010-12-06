/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/executable/timer.hpp"
#include "foundation/ipc/name.hpp"
#include "foundation/ipc/semaphore.hpp"
#include "foundation/ipc/memory/client.hpp"
#include "foundation/ipc/memory/rpc_forward.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe )( ipc )( memory ) )

//--- ipc_connect ----------------------------------------------------------------------------------
link_t ipc_connect( const std::string& name, pid_t pid, time_t time )
{
    transport transport( name, transport::open );
    ipc::semaphore semaphore( name, ipc::semaphore::open );
    semaphore_lock lock( semaphore );

    rpc< link_t ( pid_t, time_t ) > link( transport, 1 );
    return link( pid, time );
}

//--- ipc_disconnect -------------------------------------------------------------------------------
void ipc_disconnect( const std::string& name, link_t link )
{
    transport transport( name, ipc::memory::transport::open );
    ipc::semaphore semaphore( name, ipc::semaphore::open );
    semaphore_lock lock( semaphore );

    rpc< void ( link_t ) > unlink( transport, 2 );
    unlink( link );
}

OOE_ANONYMOUS_END( ( ooe )( ipc )( memory ) )

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

//--- client ---------------------------------------------------------------------------------------
client::client( const std::string& name )
    : transport( 0 ), link_client( 0 )
{
    if ( name.size() + 1 > sizeof( client_data ) - sizeof( link_t ) )
        throw error::runtime( "ipc::memory::client: " ) << '"' << name << "\" is too long";

    pid_t pid = getpid();
    time_t time = timer::epoch()._0;
    link_t link = ipc_connect( name, pid, time );
    std::string link_name = ipc::link_name( pid, time, link );

    transport_ptr( new memory::transport( link_name, transport::open ) ).swap( transport );
    link_ptr( new memory::link_client( link_name, *transport ) ).swap( link_client );

    // set client data, used for migration
    client_data& data = *static_cast< client_data* >( transport->private_data() );
    data.link = link;
    std::memcpy( data.name, name.c_str(), name.size() + 1 );
}

client::~client( void )
{
    if ( link_client )
    {
        client_data& data = *static_cast< client_data* >( transport->private_data() );
        OOE_PRINT( "ipc::memory::client", ipc_disconnect( data.name, data.link ) );
    }
}

client::operator memory::transport&( void )
{
    return *transport;
}

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )
