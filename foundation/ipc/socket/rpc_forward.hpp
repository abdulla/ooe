/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

    #ifndef OOE_FOUNDATION_IPC_SOCKET_RPC_FORWARD_HPP
    #define OOE_FOUNDATION_IPC_SOCKET_RPC_FORWARD_HPP

#include "foundation/ipc/error.hpp"
#include "foundation/ipc/fundamental.hpp"
#include "foundation/ipc/io_buffer.hpp"
#include "foundation/ipc/traits.hpp"
#include "foundation/ipc/socket/client.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( socket ) )

template< typename >
    struct rpc;

//--- except ---------------------------------------------------------------------------------------
inline void except( const u8* buffer )
{
    bool executed;
    const c8* what;
    const c8* where;
    stream_read< bool, const c8*, const c8* >::call( buffer, executed, what, where );
    throw error::rpc( executed ) << what << "\n\nServer stack trace:" << where;
}

//--- result ---------------------------------------------------------------------------------------
template< typename t >
    class result
{
public:
    typedef typename no_ref< t >::type return_type;

    result( client& client, const client::iterator& i )
        : base( new base_type( client, i ) )
    {
    }

    return_type& operator ()( void ) const
    {
        if ( base->state == base_type::done )
            return base->value;

        client::byte_array array;

        if ( base->state == base_type::wait )
            array = base->client.wait( *base );

        if ( base->state == base_type::error )
            except( array );

        stream_read< t >::call( array, base->value );
        return base->value;
    }

private:
    typedef result_base< t > base_type;
    shared_ptr< base_type > base;
};

template<>
    class result< void >
{
public:
    typedef void return_type;

    result( client& client, const client::iterator& i )
        : base( new base_type( client, i ) )
    {
    }

    return_type operator ()( void )
    {
        if ( base->state == base_type::done )
            return;

        client::byte_array array;

        if ( base->state == base_type::wait )
            array = base->client.wait( *base );

        if ( base->state == base_type::error )
            except( array );
    }

private:
    typedef result_base< void > base_type;
    shared_ptr< base_type > base;
};

//--- rpc_base -------------------------------------------------------------------------------------
class rpc_base
{
protected:
    socket::client& client;
    const index_t index;

    rpc_base( socket::client& client_, index_t index_ )
        : client( client_ ), index( index_ )
    {
    }
};

//--- store_header ---------------------------------------------------------------------------------
inline up_t store_header( io_buffer& buffer, up_t size, index_t index )
{
    up_t preserve = stream_size< length_t, index_t >::call( size, index );
    up_t allocate = size + preserve;

    buffer.allocate( allocate );
    stream_write< length_t, index_t >::call( buffer.get(), size, index );
    buffer.preserve( preserve );

    return allocate;
}

OOE_NAMESPACE_END( ( ooe )( ipc )( socket ) )

    #define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
    #define BOOST_PP_FILENAME_1 "foundation/ipc/socket/rpc_forward.hpp"
    #include BOOST_PP_ITERATE()
    #undef BOOST_PP_FILENAME_1
    #undef BOOST_PP_ITERATION_LIMITS

    #endif  // OOE_FOUNDATION_IPC_SOCKET_RPC_FORWARD_HPP

#else   // BOOST_PP_IS_ITERATING

    #define LIMIT BOOST_PP_ITERATION()

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( socket ) )

//--- rpc ------------------------------------------------------------------------------------------
template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
    struct rpc< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
    : private rpc_base
{
    typedef result< r > result_type;

    rpc( socket::client& client_, index_t index_ )
        : rpc_base( client_, index_ )
    {
    }

    result_type operator ()( BOOST_PP_ENUM_BINARY_PARAMS( LIMIT, t, a ) ) const
    {
        heap_allocator allocator;
        io_buffer buffer( client.get(), client.size(), allocator );

        up_t size = stream_size< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >::
            call( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
        size = store_header( buffer, size, index );
        stream_write< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >::
            call( buffer.get() BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );

        // must insert before write to avoid race, see commit: 00337dc5
        client::iterator i = client.insert();
        buffer.preserve( 0 );
        client.write( buffer.get(), size );
        return result_type( client, i );
    }
};

OOE_NAMESPACE_END( ( ooe )( ipc )( socket ) )

    #undef LIMIT

#endif  // BOOST_PP_IS_ITERATING
