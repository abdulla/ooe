/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/pool.hpp"
#include "foundation/ipc/socket/client.hpp"
#include "foundation/ipc/socket/rpc.hpp"
#include "foundation/utility/scoped.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe )( ipc )( socket ) )

bool socket_read( ooe::socket& socket, io_buffer& buffer, up_t size )
{
    buffer.allocate( size );
    return socket.receive( buffer.get(), size ) == size;
}

void error_set( u64& notify, mutex& mutex, condition& condition )
{
    {
        lock lock( mutex );
        notify = ~u64( 0 );
    }

    condition.notify_one();
}

void error_check( u64 notify )
{
    if ( OOE_UNLIKELY( notify == ~u64( 0 ) ) )
        throw error::connection();
}

OOE_ANONYMOUS_END( ( ooe )( ipc )( socket ) )

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( socket ) )

//--- client ---------------------------------------------------------------------------------------
client::client( const address& address )
    : connect( address ), map(), in( 0 ), out( 0 ), notify( 0 ), mutex(), condition(),
    thread( "client", make_function( *this, &client::main ), 0 ),
    scratch( new u8[ executable::static_page_size ] )
{
}

client::~client( void )
{
    if ( notify == ~u64( 0 ) )
        return;
    else if ( in != out )
        // call null function to flush pending calls
        rpc< void ( void ) >( *this, 0 )()();

    // shutdown socket to stop reader thread
    connect.shutdown( ooe::socket::read_write );
}

client::byte_array client::wait( result_type& result )
{
    map_tuple tuple;

    {
        lock lock( mutex );
        error_check( notify );

        if ( result.i->first > in )
        {
            notify = result.i->first;

            do
            {
                condition.wait( lock );
                error_check( notify );
            }
            while ( notify != result.i->first );
        }

        tuple = result.i->second;
        map.erase( result.i );
    }

    result.state = tuple._1 ? result_type::done : result_type::error;
    return tuple._0;
}

void client::erase( const iterator& i )
{
    lock lock( mutex );
    map.erase( i );
}

client::iterator client::insert( void )
{
    lock lock( mutex );
    return map.insert( map.end(), std::make_pair( ++out, map_tuple() ) );
}

u8* client::get( void ) const
{
    return scratch;
}

up_t client::size( void ) const
{
    return executable::static_page_size;
}

void client::write( const u8* data, up_t size_ )
{
    if ( connect.send( data, size_ ) != size_ )
        throw error::runtime( "ipc::socket::client: " ) << "Unable to write " << size_ << " bytes";
}

void client::main( void* )
{
    scoped< void ( u64&, ooe::mutex&, ooe::condition& ) >
        scoped( error_set, notify, mutex, condition );

    heap_allocator allocator;
    io_buffer buffer( 0, 0, allocator );
    ipc::pool pool;

    u8 header[ sizeof( length_t ) + sizeof( index_t ) ];
    length_t length = 0;
    error_t error = 0;
    up_t preserve = stream_size< length_t, error_t >::call( length, error );

    while ( true )
    {
        if ( OOE_UNLIKELY( connect.receive( header, preserve ) != preserve ) )
            break;

        stream_read< length_t, error_t >::call( header, length, error );

        if ( length && OOE_UNLIKELY( !socket_read( connect, buffer, length ) ) )
                throw error::runtime( "ipc::socket::client: " ) <<
                    "Unable to read " << length << " bytes";

        bool do_notify;

        {
            lock lock( mutex );
            map_type::iterator i = map.find( ++in );
            do_notify = in == notify;

            if ( i == map.end() )
                // result has been destructed, free memory
                delete[] allocator.release();
            else
                // result exists, store data within map
                i->second = map_tuple( byte_array( allocator.release() ), error == error::none );
        }

        // notify any waiting results
        if ( do_notify )
            condition.notify_one();
    }
}

OOE_NAMESPACE_END( ( ooe )( ipc )( socket ) )
