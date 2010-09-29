/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cerrno>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "foundation/io/error.hpp"
#include "foundation/io/socket.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

struct control_message
    : public cmsghdr
{
    s32 fd;
};

s32 family( internet_query::type type )
{
    switch ( type )
    {
    case internet_query::any:
        return AF_UNSPEC;

    case internet_query::ipv4:
        return AF_INET;

    case internet_query::ipv6:
        return AF_INET6;

    default:
        throw error::io( "internet_query: " ) << "Unknown type: " << type;
    }
}

const descriptor& check( const descriptor& desc )
{
    if ( desc.type() != descriptor::socket )
        throw error::io( "socket: " ) << "Descriptor is not a socket";

    return desc;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- socket ---------------------------------------------------------------------------------------
socket::socket( const ooe::descriptor& desc )
    : platform::socket( check( desc ) )
{
}

up_t socket::receive( void* buffer, up_t bytes )
{
    sp_t received = recv( get(), buffer, bytes, MSG_WAITALL );

    if ( received == -1 )
        throw error::io( "socket: " ) << "Unable to receive: " << error::number( errno );

    return received;
}

up_t socket::send( const void* buffer, up_t bytes )
{
    sp_t sent = ::send( get(), buffer, bytes, OOE_SOCKET_SEND_FLAG );

    if ( sent == -1 )
        throw error::io( "socket: " ) << "Unable to send: " << error::number( errno );

    return sent;
}

void socket::shutdown( shutdown_type type_ )
{
    s32 how;

    switch ( type_ )
    {
    case read:
        how = SHUT_RD;
        break;

    case write:
        how = SHUT_WR;
        break;

    case read_write:
        how = SHUT_RDWR;
        break;

    default:
        throw error::io( "socket: " ) << "Unknown shutdown type: " << type_;
    };

    if ( ::shutdown( get(), how ) && errno != ENOTCONN )
        throw error::io( "socket: " ) << "Unable to shutdown: " << error::number( errno );
}

void socket::option( u32 key, u32 value )
{
    if ( setsockopt( get(), SOL_SOCKET, key, &value, sizeof( u32 ) ) )
        throw error::io( "socket: " ) << "Unable to set option: " << error::number( errno );
}

descriptor socket::receive( void )
{
    msghdr message;
    iovec vector;
    control_message payload;

    memset( &message, 0, sizeof( message ) );
    message.msg_iov = &vector;
    message.msg_iovlen = 1;
    message.msg_control = &payload;
    message.msg_controllen = sizeof( payload );

    u8 dummy;
    vector.iov_base = &dummy;
    vector.iov_len = 1;

    if ( recvmsg( get(), &message, MSG_WAITALL ) == -1 )
        throw error::io( "socket: " ) << "Unable to receive descriptor: " << error::number( errno );
    else if ( payload.cmsg_type != SCM_RIGHTS )
        throw error::io( "socket: " ) << "Received unknown type: " << payload.cmsg_type;

    return descriptor( payload.fd );
}

void socket::send( const ooe::descriptor& desc )
{
    msghdr message;
    iovec vector;
    control_message payload;

    memset( &message, 0, sizeof( message ) );
    message.msg_iov = &vector;
    message.msg_iovlen = 1;
    message.msg_control = &payload;
    message.msg_controllen = sizeof( payload );

    u8 dummy = 0;
    vector.iov_base = &dummy;
    vector.iov_len = 1;

    memset( &payload, 0, sizeof( payload ) );
    payload.cmsg_level = SOL_SOCKET;
    payload.cmsg_type = SCM_RIGHTS;
    payload.cmsg_len = sizeof( payload );
    payload.fd = desc.get();

    if ( sendmsg( get(), &message, 0 ) == -1 )
        throw error::io( "socket: " ) << "Unable to send descriptor: " << error::number( errno );
}

//--- make_pair ------------------------------------------------------------------------------------
socket_pair make_pair( void )
{
    s32 fd[ 2 ];

    if ( socketpair( AF_LOCAL, SOCK_STREAM, 0, fd ) )
        throw error::io( "make_pair: " ) << "Unable to create socket: " << error::number( errno );

    return socket_pair( descriptor( fd[ 0 ] ), descriptor( fd[ 1 ] ) );
}

//--- listen ---------------------------------------------------------------------------------------
listen::listen( const address& address )
    : socket( descriptor( ::socket( address.family(), SOCK_STREAM, 0 ) ) )
{
    if ( bind( get(), address, address.size ) )
        throw error::io( "listen: " ) << "Unable to bind: " << error::number( errno );
    else if ( ::listen( get(), 0 ) )
        throw error::io( "listen: " ) << "Unable to listen: " << error::number( errno );
}

socket listen::accept( void ) const
{
    return ooe::descriptor( ::accept( get(), 0, 0 ) );
}

//--- connect --------------------------------------------------------------------------------------
connect::connect( const address& address )
    : socket( descriptor( ::socket( address.family(), SOCK_STREAM, 0 ) ) )
{
    ::connect( get(), address, address.size );
}

//--- address --------------------------------------------------------------------------------------
address::address( up_t size_ )
    : size( size_ ), data( new u8[ size ] )
{
    std::memset( data, 0, size );
}

u16 address::family( void ) const
{
    return data.as< sockaddr >()->sa_family;
}

address::operator const sockaddr*( void ) const
{
    return data.as< sockaddr >();
}

//--- local_address --------------------------------------------------------------------------------
local_address::local_address( const std::string& path )
    : address( sizeof( u16 ) + path.size() + 1 )
{
    sockaddr_un& sa = *data.as< sockaddr_un >();
    sa.sun_family = AF_LOCAL;
    std::memcpy( sa.sun_path, path.c_str(), path.size() + 1 );
}

//--- internet_address -----------------------------------------------------------------------------
internet_address::internet_address( const addrinfo& ai )
    : address( ai.ai_addrlen )
{
    std::memcpy( data, ai.ai_addr, ai.ai_addrlen );
}

//--- internet_query_iterator ----------------------------------------------------------------------
internet_query_iterator::internet_query_iterator( const addrinfo* node_ )
    : node( node_ )
{
}

void internet_query_iterator::increment( void )
{
    node = node->ai_next;
}

bool internet_query_iterator::equal( const internet_query_iterator& other ) const
{
    return node == other.node;
}

internet_address internet_query_iterator::dereference( void ) const
{
    return *node;
}

//--- internet_query -------------------------------------------------------------------------------
internet_query::internet_query( const std::string& host, const std::string& service, type flag )
    : head()
{
    addrinfo hint;
    memset( &hint, 0, sizeof( hint ) );
    hint.ai_family = family( flag );
    hint.ai_socktype = SOCK_STREAM;

    const c8* host_name = host.empty() ? 0 : host.c_str();
    const c8* service_name = service.empty() ? 0 : service.c_str();
    s32 status = getaddrinfo( host_name, service_name, &hint, &head );

    if ( status )
        throw error::io( "internet_query: " ) << "Unable to resolve \"" << host << "\" with \"" <<
            service << "\": " << gai_strerror( status );
}

internet_query::~internet_query( void )
{
    freeaddrinfo( head );
}

internet_query::iterator internet_query::begin( void ) const
{
    return head;
}

internet_query::iterator internet_query::end( void ) const
{
    return 0;
}

OOE_NAMESPACE_END( ( ooe ) )
