/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "foundation/io/poll.hpp"
#include "foundation/ipc/memory/link.hpp"
#include "foundation/ipc/memory/server.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

//--- link_server ----------------------------------------------------------------------------------
link_server::link_server( const ooe::socket& socket_, servlet_iterator iterator_,
    memory::server& server_, transport& transport )
    : socket( socket_ ), iterator( iterator_ ), server( server_ ), state( true ),
    thread( "link_server", make_function( *this, &link_server::main ), &transport )
{
}

link_server::operator bool( void ) const
{
    return state;
}

void link_server::main( void* pointer )
{
    poll poll;
    poll.insert( socket );
    poll.wait();

    if ( state.exchange( false ) )
        server.erase( iterator );

    // wake servlet and indicate that it should call null and exit
    memory::transport& transport = *static_cast< memory::transport* >( pointer );
    stream_write< bool_t, index_t >::call( transport.get(), true, 0 );
    transport.wake_wait();
}

//--- link_client ----------------------------------------------------------------------------------
link_client::link_client( const ooe::socket& socket_, transport& transport )
    : socket( socket_ ), state( true ),
    thread( "link_client", make_function( *this, &link_client::main ), &transport )
{
}

link_client::~link_client( void )
{
    if ( state.exchange( false ) )
        socket.shutdown( socket::read );
}

link_client::operator bool( void ) const
{
    return state;
}

void link_client::main( void* pointer )
{
    poll poll;
    poll.insert( socket );
    poll.wait();

    if ( !state.exchange( false ) )
        return;

    // wake client and indicate that an error in the link has occurred
    memory::transport& transport = *static_cast< memory::transport* >( pointer );
    stream_write< bool_t, error_t, const c8* >::call( transport.get(), true, error::link, "" );
    transport.wake_notify();
}

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )
