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

			inline bool header_size( ooe::socket&, u32& );
			inline array_type header_read( ooe::socket&, u32 );
			inline void header_read( ooe::socket&, u32, buffer_type& );
		}
	}

//--- ipc::socket --------------------------------------------------------------
	inline bool ipc::socket::header_size( ooe::socket& socket, u32& size )
	{
		return socket.receive( &size, sizeof( u32 ) ) == sizeof( u32 );
	}

	inline ipc::socket::array_type ipc::socket::header_read( ooe::socket& socket, u32 size )
	{
		if ( !size )
			return array_type();

		array_type array( new u8[ size ] );

		if ( socket.receive( array, size ) != size )
			throw error::runtime( "ipc::socket::header_read: " ) << "Unable to read data";

		return array;
	}

	inline void ipc::socket::header_read( ooe::socket& socket, u32 size, buffer_type& buffer )
	{
		if ( !size )
			return;

		buffer.reserve( size );

		if ( socket.receive( &buffer[ 0 ], size ) != size )
			throw error::runtime( "ipc::socket::header_read: " ) << "Unable to read data";
	}
}

#endif	// OOE_FOUNDATION_IPC_SOCKET_HEADER_HPP
