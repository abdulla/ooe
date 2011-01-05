/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/io/poll.hpp"
#include "foundation/ipc/memory/link.hpp"
#include "foundation/ipc/memory/server.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

//--- link_server ----------------------------------------------------------------------------------
link_server::link_server( const ooe::socket& socket_, link_t link_, server& server )
    : pair( make_pair() ), socket( socket_ ), link( link_ ), state( true ),
    thread( "link_server", make_function( *this, &link_server::main ), &server )
{
}

link_server::~link_server( void )
{
    if ( !state.exchange( false ) )
        return;

    pair._1.shutdown( socket::write );
    thread.join();
}

void* link_server::main( void* pointer )
{
    memory::server& server = *static_cast< memory::server* >( pointer );

    poll poll;
    poll.insert( socket );
    poll.insert( pair._0 );
    poll.wait();

    if ( state.exchange( false ) )
        server.unlink( link );

    return 0;
}

//--- link_client ----------------------------------------------------------------------------------
link_client::link_client( const ooe::socket& socket_, transport& transport )
    : pair( make_pair() ), socket( socket_ ), state( true ),
    thread( "link_client", make_function( *this, &link_client::main ), &transport )
{
}

link_client::~link_client( void )
{
    shutdown();
    thread.join();
}

void link_client::shutdown( void )
{
    if ( state.exchange( false ) )
        pair._1.shutdown( socket::write );
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
    poll.insert( pair._0 );
    poll.wait();

    if ( !state.exchange( false ) )
        return 0;

    // wake client and indicate that an error in the link has occurred
    stream_write< bool_t, error_t, const c8* >::call( transport.get(), true, error::link, "" );
    transport.wake_notify();
    return 0;
}

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )
