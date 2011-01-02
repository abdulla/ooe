/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <cerrno>

#include "foundation/io/directory.hpp"
#include "foundation/io/poll.hpp"
#include "foundation/ipc/memory/link.hpp"
#include "foundation/ipc/memory/server.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

void shutdown( ooe::socket& socket, atom< bool >& state )
{
    if ( state.exchange( false ) )
        socket.shutdown( socket::write );
}

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

//--- link_listen ----------------------------------------------------------------------------------
link_listen::link_listen( const std::string& name )
    : path( local_name( name ) ), listen( local_address( path ) )
{
}

link_listen::~link_listen( void )
{
    OOE_PRINT( "ipc::link_listen", erase( path ) );
}

socket link_listen::accept( void ) const
{
    return listen.accept();
}

//--- link_server ----------------------------------------------------------------------------------
link_server::link_server( const ooe::socket& socket_, link_t link_, server& server )
    : socket( socket_ ), pair( make_pair() ), link( link_ ), state( true ),
    thread( "link_server", make_function( *this, &link_server::main ), &server )
{
}

link_server::~link_server( void )
{
    shutdown( pair._1, state );
    thread.join();
}

void link_server::migrate( ooe::socket& migrate_socket )
{
    migrate_socket.send( socket );
    shutdown( pair._1, state );
}

void* link_server::main( void* pointer )
{
    memory::server& server = *static_cast< memory::server* >( pointer );

    poll poll;
    poll.insert( socket );
    poll.insert( pair._0 );
    poll.wait();

    if ( state.exchange( false ) )
        server.unlink( link, true );

    return 0;
}

//--- link_client ----------------------------------------------------------------------------------
link_client::link_client( const std::string& name, transport& transport )
    : connect( local_address( local_name( name ) ) ), pair( make_pair() ), state( true ),
    thread( "link_client", make_function( *this, &link_client::main ), &transport )
{
}

link_client::~link_client( void )
{
    ::shutdown( pair._1, state );
    thread.join();
}

void link_client::shutdown( void )
{
    ::shutdown( pair._1, state );
}

link_client::operator bool( void ) const
{
    return state;
}

void* link_client::main( void* pointer )
{
    memory::transport& transport = *static_cast< memory::transport* >( pointer );

    poll poll;
    poll.insert( connect );
    poll.insert( pair._0 );
    poll.wait();

    if ( !state.exchange( false ) )
        return 0;

    // wake for result, indicating that an error in the link has occurred
    stream_write< bool_t, error_t, const c8* >::call( transport.get(), true, error::link, "" );
    transport.wake_notify();
    return 0;
}

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )
