/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_SOCKET_HEADER_HPP
#define OOE_FOUNDATION_IPC_SOCKET_HEADER_HPP

#include <vector>

#include "foundation/io/socket.hpp"
#include "foundation/utility/error.hpp"

namespace ooe
{
	namespace ipc
	{
		namespace socket
		{
			typedef shared_array< u8 > array_type;
			typedef std::vector< u8 > buffer_type;

			bool header_size( ooe::socket&, u32& );
			array_type header_read( ooe::socket&, u32 );
			void header_read( ooe::socket&, buffer_type&, u32 );
			void header_write( ooe::socket&, u8*, u32 );
		}
	}

//--- ipc::socket --------------------------------------------------------------
	bool ipc::socket::header_size( ooe::socket& socket, u32& size )
	{
		return socket.receive( &size, sizeof( u32 ) ) == sizeof( u32 );
	}

	ipc::socket::array_type ipc::socket::header_read( ooe::socket& socket, u32 size )
	{
		if ( !size )
			return array_type();

		array_type array( new u8[ size ] );

		if ( socket.receive( array, size ) != size )
			throw error::runtime( "nipc::header_read: " ) << "Unable to read data";

		return array;
	}

	void ipc::socket::header_read( ooe::socket& socket, u32 size, buffer_type& buffer )
	{
		if ( !size )
			return;

		buffer.reserve( size );

		if ( socket.receive( &buffer[ 0 ], size ) != size )
			throw error::runtime( "nipc::header_read: " ) << "Unable to read data";
	}

	void ipc::socket::header_write( ooe::socket& socket, u8* buffer, u32 size )
	{
		// make space for u32 in buffer, write size in to it, do single send
	}
}

#endif	// OOE_FOUNDATION_IPC_SOCKET_HEADER_HPP
