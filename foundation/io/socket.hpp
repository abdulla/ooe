/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IO_SOCKET_HPP
#define OOE_FOUNDATION_IO_SOCKET_HPP

#include <boost/iterator/iterator_facade.hpp>

#include "foundation/io/socket_forward.hpp"
#include "foundation/utility/tuple.hpp"

struct sockaddr;
struct addrinfo;

OOE_NAMESPACE_BEGIN( ( ooe ) )

class address;

//--- socket ---------------------------------------------------------------------------------------
struct OOE_VISIBLE socket
    : public platform::socket
{
    enum shutdown_type
    {
        read,
        write,
        read_write
    };

    socket( const ooe::descriptor& );

    up_t receive( void*, up_t );
    up_t send( const void*, up_t );

    void shutdown( shutdown_type );
    void option( u32, u32 );

    ooe::descriptor receive( void );
    void send( const ooe::descriptor& );
};

//--- make_pair ------------------------------------------------------------------------------------
typedef tuple< socket, socket > socket_pair;
socket_pair make_pair( void ) OOE_VISIBLE;

//--- listen ---------------------------------------------------------------------------------------
struct OOE_VISIBLE listen
    : public socket
{
    listen( const address& );

    socket accept( void ) const;
};

//--- connect --------------------------------------------------------------------------------------
struct OOE_VISIBLE connect
    : public socket
{
    connect( const address& );
};

//--- address --------------------------------------------------------------------------------------
class address
{
public:
    const up_t size;

    u16 family( void ) const;
    operator const sockaddr*( void ) const;

protected:
    const shared_array< u8 > data;

    address( up_t );
};

//--- local_address --------------------------------------------------------------------------------
struct local_address
    : public address
{
    local_address( const std::string& ) OOE_VISIBLE;
};

//--- internet_address -----------------------------------------------------------------------------
struct internet_address
    : public address
{
    internet_address( const addrinfo& ) OOE_VISIBLE;
};

//--- internet_query_iterator ----------------------------------------------------------------------
class internet_query_iterator
    : public boost::iterator_facade< internet_query_iterator, internet_address,
    boost::forward_traversal_tag, internet_address >
{
private:
    const addrinfo* node;

    internet_query_iterator( const addrinfo* );

    void increment( void ) OOE_VISIBLE;
    bool equal( const internet_query_iterator& ) const OOE_VISIBLE;
    internet_address dereference( void ) const OOE_VISIBLE;

    friend class internet_query;
    friend class boost::iterator_core_access;
};

//--- internet_query -------------------------------------------------------------------------------
class OOE_VISIBLE internet_query
    : private noncopyable
{
public:
    enum type
    {
        any,
        ipv4,
        ipv6
    };

    typedef internet_query_iterator iterator;

    internet_query( const std::string&, const std::string&, type = any );
    ~internet_query( void );

    iterator begin( void ) const;
    iterator end( void ) const;

private:
    addrinfo* head;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_IO_SOCKET_HPP
