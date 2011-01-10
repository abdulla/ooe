/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/memory/server.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe )( ipc )( memory ) )

typedef tuple< const switchboard&, shared_allocator&, io_buffer&, ipc::pool& > tuple_type;

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

    switchboard::size_type size = tuple._0.execute( index, buffer, tuple._3 );

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

OOE_ANONYMOUS_END( ( ooe )( ipc )( memory ) )

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

//--- servlet --------------------------------------------------------------------------------------
servlet::servlet( const ooe::socket& socket_, const ipc::switchboard& switchboard_,
    servlet_iterator iterator_, server& server )
    : socket( socket_ ), switchboard( switchboard_ ), iterator( iterator_ ), detached( false ),
    thread( "servlet", make_function( *this, &servlet::main ), &server )
{
}

servlet::~servlet( void )
{
    if ( detached )
        return;

    socket.shutdown( socket::read );
    thread.join();
}

void servlet::detach( void )
{
    detached = true;
}

void* servlet::main( void* pointer )
{
    server& server = *static_cast< memory::server* >( pointer );
    transport transport( socket );
    link_server link( socket, iterator, server, transport );

    shared_allocator allocator;
    io_buffer buffer( transport.get(), transport.size(), allocator );
    pool pool;
    tuple_type tuple( switchboard, allocator, buffer, pool );

    while ( OOE_LIKELY( link ) )
        transport.wait( ipc_decode, &tuple );

    return 0;
}

//--- server ---------------------------------------------------------------------------------------
server::server( const ipc::switchboard& switchboard_ )
    : switchboard( switchboard_ ), mutex(), list()
{
}

server::~server( void )
{
}

void server::insert( const socket& socket )
{
    lock lock( mutex );
    list.push_front( servlet_ptr() );
    list.front() = servlet_ptr( new servlet( socket, switchboard, list.begin(), *this ) );
}

void server::erase( servlet_iterator iterator )
{
    ( *iterator )->detach();
    lock lock( mutex );
    list.erase( iterator );
}

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )
