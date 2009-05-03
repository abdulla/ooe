/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_SOCKET_HEADER_HPP
#define OOE_FOUNDATION_IPC_SOCKET_HEADER_HPP

#include <vector>

#include "foundation/io/socket.hpp"

namespace ooe
{
	namespace nipc
	{
		typedef shared_array< u8 > array_type;
		typedef std::vector< u8 > buffer_type;

		bool header_size( socket&, u32& );
		array_type header_read( socket&, u32 );
		void header_read( socket&, buffer_type&, u32 );
	}
}

#endif	// OOE_FOUNDATION_IPC_SOCKET_HEADER_HPP
