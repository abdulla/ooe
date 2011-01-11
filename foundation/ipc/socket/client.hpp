/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_SOCKET_CLIENT_HPP
#define OOE_FOUNDATION_IPC_SOCKET_CLIENT_HPP

#include <map>

#include "foundation/io/socket.hpp"
#include "foundation/parallel/lock.hpp"
#include "foundation/parallel/thread.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( socket ) )

template< typename >
    struct result_base;

//--- client ---------------------------------------------------------------------------------------
class OOE_VISIBLE client
{
public:
    typedef shared_array< u8 > byte_array;
    typedef tuple< byte_array, bool > map_tuple;
    typedef std::map< u64, map_tuple > map_type;
    typedef map_type::iterator iterator;
    typedef result_base< void > result_type;

    client( const address& );
    ~client( void );

    byte_array wait( result_type& );
    void erase( const iterator& );
    iterator insert( void );

    u8* get( void ) const;
    up_t size( void ) const;
    void write( const u8*, up_t );

private:
    ooe::connect connect;

    map_type map;
    u64 in;
    u64 out;
    u64 notify;

    ooe::mutex mutex;
    ooe::condition condition;
    ooe::thread thread;

    scoped_array< u8 > scratch;

    void main( void* ) OOE_HIDDEN;
};

//--- result_base ----------------------------------------------------------------------------------
template<>
    struct OOE_VISIBLE result_base< void >
    : private noncopyable
{
    enum state_type
    {
        wait,
        done,
        error
    };

    typedef socket::client client_type;
    typedef client_type::map_type::iterator iterator;

    client_type& client;
    const iterator i;
    state_type state;

    result_base( client_type& client_, const iterator& i_ )
        : client( client_ ), i( i_ ), state( wait )
    {
    }

    ~result_base( void )
    {
        if ( state == wait )
            client.erase( i );
    }
};

template< typename t >
    struct result_base
    : public result_base< void >
{
    t value;

    result_base( client_type& client_, const iterator& i_ )
        : result_base< void >( client_, i_ ), value()
    {
    }
};

OOE_NAMESPACE_END( ( ooe )( ipc )( socket ) )

#endif  // OOE_FOUNDATION_IPC_SOCKET_CLIENT_HPP
