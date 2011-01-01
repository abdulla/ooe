/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/executable/program.hpp"
#include "foundation/ipc/name.hpp"
#include "foundation/ipc/memory/client.hpp"
#include "foundation/ipc/memory/server.hpp"
#include "foundation/ipc/memory/server_private.hpp"
#include "foundation/utility/scoped.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe )( ipc )( memory ) )

typedef tuple< const switchboard&, shared_allocator&, io_buffer&, ipc::pool* > tuple_type;

OOE_TLS( servlet* ) servlet_tls;
up_t active_servers;
executable::signal_handler_type prior_handler;

//--- signal_handler -------------------------------------------------------------------------------
void signal_handler( s32 code, siginfo_t* info, void* context )
{
    servlet* servlet = servlet_tls;

    if ( servlet )
        servlet->check( info->si_addr );

    prior_handler( code, info, context );
}

//--- ipc_decode -----------------------------------------------------------------------------------
void ipc_decode( const void* pointer )
{
    const tuple_type& tuple = *static_cast< const tuple_type* >( pointer );
    shared_allocator& allocator = tuple._1;
    io_buffer& buffer = tuple._2;
    buffer.reset();
    u8* data = buffer.get();

    bool_t internal;
    index_t index;
    const c8* name;
    stream_read< bool_t, index_t, const c8* >::call( data, internal, index, name );

    if ( OOE_LIKELY( internal ) )
    {
        up_t preserve = stream_size< bool_t, index_t >::call( internal, index );
        buffer.preserve( preserve );
    }
    else
    {
        allocator.set( name );
        buffer.external();
    }

    switchboard::size_type size = tuple._0.execute( index, buffer, *tuple._3 );

    buffer.preserve( 0 );
    internal = buffer.is_internal();
    ipc::error_t error = size._1 ? error::exception : error::none;

    if ( OOE_LIKELY( internal ) )
    {
        stream_write< bool_t, ipc::error_t >::call( data, internal, error );
        allocator.deallocate();
    }
    else
        stream_write< bool_t, ipc::error_t, std::string >::
            call( data, internal, error, allocator.name() );
}

//--- ipc_link -------------------------------------------------------------------------------------
up_t ipc_link( const any& any, io_buffer& buffer, pool& pool )
{
    pid_t pid;
    time_t time;
    stream_read< pid_t, time_t >::call( buffer.get(), pid, time );

    link_t link = static_cast< server* >( any.pointer )->link( pid, time );
    return return_write< link_t >( buffer, pool, link );
}

//--- ipc_unlink -----------------------------------------------------------------------------------
up_t ipc_unlink( const any& any, io_buffer& buffer, pool& )
{
    link_t link;
    stream_read< link_t >::call( buffer.get(), link );

    static_cast< server* >( any.pointer )->unlink( link, true );
    return 0;
}

OOE_ANONYMOUS_END( ( ooe )( ipc )( memory ) )

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

//--- servlet --------------------------------------------------------------------------------------
servlet::servlet( const std::string& link_name, link_t link_, const ipc::switchboard& switchboard_,
    server& server )
    : transport( link_name, transport::create ), link( link_ ), switchboard( switchboard_ ),
    link_listen( new memory::link_listen( link_name ) ), link_server( 0 ), allocator(),
    buffer( transport.get(), transport.size(), allocator ), state( true ),
    thread( "servlet", make_function( *this, &servlet::main ), &server )
{
}

servlet::servlet( socket& socket, link_t link_, const ipc::switchboard& switchboard_,
    server& server )
    : transport( socket ), link( link_ ), switchboard( switchboard_ ), link_listen( 0 ),
    link_server( new memory::link_server( socket.receive(), link, server ) ), allocator(),
    buffer( transport.get(), transport.size(), allocator ), state( true ),
    thread( "servlet", make_function( *this, &servlet::main ), &server )
{
    std::string name = server.name();

    // rewrite connection data in transport
    client_data& data = *static_cast< client_data* >( transport.private_data() );
    data.link = link;
    std::memcpy( data.name, name.c_str(), name.size() + 1 );
}

void servlet::join( void )
{
    // wake for arguments, indicating that the servlet should call null and exit
    state.exchange( false );
    stream_write< bool_t, index_t >::call( transport.get(), true, 0 );
    transport.wake_wait();
    thread.join();
}

void servlet::migrate( socket& socket, server& server )
{
    transport.migrate( socket );
    link_server->migrate( socket );
    state.exchange( false );
    server.unlink( link, false );
}

void servlet::check( const void* address )
{
    bool inside =
        buffer.is_internal() ? transport.in_canary( address ) : allocator.in_canary( address );

    if ( inside )
        canary_handler( address );
}

void* servlet::main( void* pointer )
{
    memory::server& server = *static_cast< memory::server* >( pointer );
    atom_ptr< servlet > guard = server.find( link );

    if ( link_listen )
    {
        const socket& socket = link_listen->accept();
        scoped_ptr< memory::link_server >
            ( new memory::link_server( socket, link, server ) ).swap( link_server );
        delete link_listen.release();
        transport.unlink();
    }

    pool pool;
    tuple_type tuple( switchboard, allocator, buffer, &pool );
    servlet_tls = this;

    while ( OOE_LIKELY( state ) )
        transport.wait( ipc_decode, &tuple );

    return 0;
}

//--- server ---------------------------------------------------------------------------------------
server::server( const std::string& name_, const switchboard& external_ )
    : semaphore( name_, semaphore::create ), transport( name_, transport::create ),
    external( external_ ), internal(), seed(), mutex(), map()
{
    if ( name_.size() + 1 > sizeof( client_data ) - sizeof( link_t ) )
        throw error::runtime( "ipc::memory::server: " ) << '"' << name_ << "\" is too long";
    else if ( internal.insert_direct( ipc_link, this ) != 1 )
        throw error::runtime( "ipc::memory::server: " ) << "\"link\" not at index 1";
    else if ( internal.insert_direct( ipc_unlink, this ) != 2 )
        throw error::runtime( "ipc::memory::server: " ) << "\"unlink\" not at index 2";

    if ( !active_servers++ )
    {
        struct sigaction action;
        prior_handler = executable::signal( action, signal_handler, OOE_SIGFAULT );
    }
}

server::~server( void )
{
    if ( !--active_servers )
    {
        struct sigaction action;
        executable::signal( action, prior_handler, OOE_SIGFAULT );
    }

    for ( servlet_map::iterator i = map.begin(), end = map.end(); i != end; ++i )
        i->second->join();
}

std::string server::name( void ) const
{
    return transport.name();
}

bool server::decode( void )
{
    shared_allocator allocator;
    io_buffer buffer( transport.get(), transport.size(), allocator );
    tuple_type tuple( internal, allocator, buffer, 0 );
    transport.wait( ipc_decode, &tuple );

    lock lock( mutex );
    return !map.empty();
}

link_t server::link( pid_t pid, time_t time )
{
    link_t id = seed++;
    std::string link_name = ipc::link_name( pid, time, id );

    lock lock( mutex );
    map.insert( map.end(), std::make_pair( id, new servlet( link_name, id, external, *this ) ) );
    return id;
}

void server::unlink( link_t id, bool join )
{
    lock lock( mutex );
    servlet_map::iterator i = map.find( id );

    if ( i == map.end() )
        throw error::runtime( "ipc::memory::server: " ) << "Servlet " << id << " does not exist";
    else if ( join )
        i->second->join();

    map.erase( i );
}

atom_ptr< servlet > server::find( link_t id ) const
{
    lock lock( mutex );
    servlet_map::const_iterator i = map.find( id );

    if ( i == map.end() )
        throw error::runtime( "ipc::memory::server: " ) << "Servlet " << id << " does not exist";

    return i->second;
}

void server::migrate( socket& socket )
{
    servlet_tls->migrate( socket, *this );
}

void server::relink( socket& socket )
{
    link_t id = seed++;
    servlet_map::value_type value( id, new servlet( socket, id, external, *this ) );

    lock lock( mutex );
    map.insert( map.end(), value );
}

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )
