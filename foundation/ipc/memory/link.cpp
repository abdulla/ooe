/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/io/poll.hpp"
#include "foundation/ipc/memory/link.hpp"
#include "foundation/ipc/memory/server.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

//--- link_server ----------------------------------------------------------------------------------
link_server::link_server( const ooe::socket& socket_, servlet_iterator iterator_,
    memory::server& server_, atom< bool >& state_, transport& transport )
    : socket( socket_ ), iterator( iterator_ ), server( server_ ), state( state_ ),
    thread( "link_server", make_function( *this, &link_server::main ), &transport )
{
}

link_server::~link_server( void )
{
    thread.join();
}

void* link_server::main( void* pointer )
{
    memory::transport& transport = *static_cast< memory::transport* >( pointer );

    poll poll;
    poll.insert( socket );
    poll.wait();

    if ( state.exchange( false ) )
        server.erase( iterator );

    // wake servlet and indicate that it should call null and exit
    stream_write< bool_t, index_t >::call( transport.get(), true, 0 );
    transport.wake_wait();
    return 0;
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

    thread.join();
}

link_client::operator bool( void ) const
{
    return state;
}

void* link_client::main( void* pointer )
{
    memory::transport& transport = *static_cast< memory::transport* >( pointer );

    poll poll;
    poll.insert( socket );
    poll.wait();

    if ( !state.exchange( false ) )
        return 0;

    // wake client and indicate that an error in the link has occurred
    stream_write< bool_t, error_t, const c8* >::call( transport.get(), true, error::link, "" );
    transport.wake_notify();
    return 0;
}

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )
