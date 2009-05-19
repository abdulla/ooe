/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <cerrno>

#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "foundation/io/error.hpp"
#include "foundation/io/socket.hpp"

namespace
{
	using namespace ooe;

	hostent* lookup( const std::string& path, s32 family, const c8* name )
	{
		hostent* he = gethostbyname2( path.c_str(), family );

		if ( !he )
			throw error::io( name ) << ": Unable to resolve \"" << path << "\": " <<
				hstrerror( h_errno );

		return he;
	}
}

namespace ooe
{
//--- socket -------------------------------------------------------------------
	socket::socket( s32 fd )
		: platform::socket( fd )
	{
	}

	socket socket::create( s32 fd ) const
	{
		return socket( fd );
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

	socket::poll_type socket::poll( void ) const
	{
		struct pollfd event = { get(), POLLIN, 0 };

		if ( ::poll( &event, 1, -1 ) == -1 )
			throw error::io( "socket: " ) << "Unable to poll: " << error::number( errno );
		else if ( event.revents & POLLIN )
			return input;
		else if ( event.revents & POLLHUP )
			return hang_up;
		else if ( event.revents & POLLERR )
			return error;
		else
			throw error::io( "socket: " ) << "Unknown poll event";
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
			throw error::io( "socket: " ) << "Unknown shutdown type";
		};

		if ( ::shutdown( get(), how ) && errno != ENOTCONN )
			throw error::io( "socket: " ) << "Unable to shutdown: " << error::number( errno );
	}

	void socket::option( u32 key, u32 value )
	{
		if ( setsockopt( get(), SOL_SOCKET, key, &value, sizeof( u32 ) ) )
			throw error::io( "socket: " ) << "Unable to set option: " << error::number( errno );
	}

//--- listen -------------------------------------------------------------------
	listen::listen( const address& address )
		: socket( ::socket( address.family(), SOCK_STREAM, 0 ) )
	{
		if ( bind( get(), address, address.size ) )
			throw error::io( "listen: " ) << "Unable to bind: " << error::number( errno );
		else if ( ::listen( get(), 0 ) )
			throw error::io( "listen: " ) << "Unable to listen: " << error::number( errno );
	}

	socket listen::accept( void ) const
	{
		return create( ::accept( get(), 0, 0 ) );
	}

//--- connect ------------------------------------------------------------------
	connect::connect( const address& address )
		: socket( ::socket( address.family(), SOCK_STREAM, 0 ) )
	{
		::connect( get(), address, address.size );
	}

//--- address ------------------------------------------------------------------
	address::address( up_t size_ )
		: size( size_ ), data( new u8[ size ] )
	{
		std::fill_n( data.get(), size, 0 );
	}

	u16 address::family( void ) const
	{
		return data.as< sockaddr >()->sa_family;
	}

	address::operator const sockaddr*( void ) const
	{
		return data.as< sockaddr >();
	}

//--- local --------------------------------------------------------------------
	local::local( const std::string& path )
		: address( sizeof( u16 ) + path.size() + 1 )
	{
		sockaddr_un& sa = *data.as< sockaddr_un >();
		sa.sun_family = AF_LOCAL;
		const c8* in = path.c_str();
		std::copy( in, in + path.size() + 1, sa.sun_path );
	}

//--- internet -----------------------------------------------------------------
	internet::internet( const std::string& path, u16 port )
		: address( sizeof( sockaddr_in ) )
	{
		hostent* he = lookup( path, AF_INET, "internet" );

		sockaddr_in& sa = *data.as< sockaddr_in >();
		sa.sin_family = AF_INET;
		sa.sin_port = htons( port );
		sa.sin_addr = *reinterpret_cast< in_addr* >( he->h_addr );
	}

//--- internet6 ----------------------------------------------------------------
	internet6::internet6( const std::string& path, u16 port )
		: address( sizeof( sockaddr_in6 ) )
	{
		hostent* he = lookup( path, AF_INET6, "internet6" );

		sockaddr_in6& sa = *data.as< sockaddr_in6 >();
		sa.sin6_family = AF_INET6;
		sa.sin6_port = htons( port );
		sa.sin6_addr = *reinterpret_cast< in6_addr* >( he->h_addr );
	}
}
