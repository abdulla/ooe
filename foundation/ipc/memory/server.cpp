/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/io/directory.hpp"
#include "foundation/ipc/name.hpp"
#include "foundation/ipc/memory/server.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe )( ipc )( memory ) )

typedef tuple< const switchboard&, shared_allocator&, io_buffer&, ipc::pool* > tuple_type;

//--- ipc_address ----------------------------------------------------------------------------------
address ipc_address( const std::string& server_name )
{
    std::string name = local_name( server_name );

    if ( exists( name ) )
        erase( name );

    return local_address( name );
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
    std::string client_name;
    stream_read< std::string >::call( buffer.get(), client_name );

    link_t link = static_cast< server* >( any.pointer )->link( client_name );
    return return_write< link_t >( buffer, pool, link );
}

//--- ipc_unlink -----------------------------------------------------------------------------------
up_t ipc_unlink( const any& any, io_buffer& buffer, pool& )
{
    link_t link;
    stream_read< link_t >::call( buffer.get(), link );

    static_cast< server* >( any.pointer )->unlink( link );
    return 0;
}

//--- set_transport --------------------------------------------------------------------------------
void set_transport
    ( ooe::mutex& mutex, ipc::memory::transport*& transport, ipc::memory::transport* value )
{
    lock lock( mutex );
    transport = value;
}

//--- wake_transport -------------------------------------------------------------------------------
void wake_transport( ooe::mutex& mutex, ipc::memory::transport*& transport )
{
    lock lock( mutex );

    if ( !transport )
        return;

    // wake servlet and indicate that it should call null and exit
    stream_write< bool_t, index_t >::call( transport->get(), true, 0 );
    transport->wake_wait();
}

OOE_ANONYMOUS_END( ( ooe )( ipc )( memory ) )

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

//--- servlet --------------------------------------------------------------------------------------
servlet::servlet( link_t link_, const ipc::switchboard& switchboard_, const ooe::socket& socket_,
    server& server )
    : link( link_ ), switchboard( switchboard_ ), socket( socket_ ), mutex(), transport_ptr( 0 ),
    state( true ), thread( "servlet", make_function( *this, &servlet::main ), &server )
{
}

servlet::~servlet( void )
{
    state.exchange( false );
    wake_transport( mutex, transport_ptr );
    thread.join();
}

void* servlet::main( void* pointer )
{
    server& server = *static_cast< memory::server* >( pointer );
    transport transport( socket );
    link_server link_server( socket, link, server );

    shared_allocator allocator;
    io_buffer buffer( transport.get(), transport.size(), allocator );
    pool pool;
    tuple_type tuple( switchboard, allocator, buffer, &pool );
    set_transport( mutex, transport_ptr, &transport );

    while ( OOE_LIKELY( state ) )
        transport.wait( ipc_decode, &tuple );

    set_transport( mutex, transport_ptr, 0 );
    return 0;
}

//--- server ---------------------------------------------------------------------------------------
server::server( const std::string& name_, const switchboard& external_ )
    : listen( ipc_address( name_ ) ), transport( name_ ), external( external_ ),
    internal(), seed(), read_write(), map()
{
    if ( internal.insert_direct( ipc_link, this ) != 1 )
        throw error::runtime( "ipc::memory::server: " ) << "\"link\" not at index 1";
    else if ( internal.insert_direct( ipc_unlink, this ) != 2 )
        throw error::runtime( "ipc::memory::server: " ) << "\"unlink\" not at index 2";
}

std::string server::name( void ) const
{
    return transport.name();
}

bool server::decode( void )
{
    socket socket = listen.accept();
    transport.send( socket );

    shared_allocator allocator;
    io_buffer buffer( transport.get(), transport.size(), allocator );
    tuple_type tuple( internal, allocator, buffer, 0 );
    transport.wait( ipc_decode, &tuple );

    read_lock lock( read_write );
    return !map.empty();
}

link_t server::link( const std::string& client_name )
{
    connect connect( local_address( local_name( client_name ) ) );
    link_t id = seed++;

    write_lock lock( read_write );
    map.insert( map.end(), std::make_pair( id, new servlet( id, external, connect, *this ) ) );
    return id;
}

void server::unlink( link_t id )
{
    write_lock write_lock( read_write );
    map.erase( id );
}

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )
