/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IO_SOCKET_HPP
#define OOE_FOUNDATION_IO_SOCKET_HPP

#include "foundation/io/socket_forward.hpp"

struct sockaddr;

namespace ooe
{
	struct address;

//--- socket -------------------------------------------------------------------
	struct OOE_VISIBLE socket
		: public platform::socket
	{
		enum poll_type
		{
			input,
			hang_up,
			error
		};

		enum shutdown_type
		{
			read,
			write,
			read_write
		};

		socket( const ooe::descriptor& );

		up_t receive( void*, up_t );
		up_t send( const void*, up_t );

		poll_type poll( void ) const;
		void shutdown( shutdown_type );
		void option( u32, u32 );

		ooe::descriptor receive( void );
		void send( const ooe::descriptor& );
	};

	typedef tuple< socket, socket > socket_pair;
	socket_pair make_pair( void );

//--- listen -------------------------------------------------------------------
	struct OOE_VISIBLE listen
		: public socket
	{
		listen( const address& );

		socket accept( void ) const;
	};

//--- connect ------------------------------------------------------------------
	struct OOE_VISIBLE connect
		: public socket
	{
		connect( const address& );
	};

//--- address ------------------------------------------------------------------
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

//--- local --------------------------------------------------------------------
	struct local
		: public address
	{
		local( const std::string& ) OOE_VISIBLE;
	};

//--- internet -----------------------------------------------------------------
	struct internet
		: public address
	{
		internet( const std::string&, u16 ) OOE_VISIBLE;
	};

//--- internet6 ----------------------------------------------------------------
	struct internet6
		: public address
	{
		internet6( const std::string&, u16 ) OOE_VISIBLE;
	};
}

#endif	// OOE_FOUNDATION_IO_SOCKET_HPP
