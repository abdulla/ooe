/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IO_SOCKET_FORWARD_HPP
#define OOE_FOUNDATION_IO_SOCKET_FORWARD_HPP

#include "foundation/io/descriptor.hpp"

#define OOE_SOCKET_SEND_FLAG MSG_NOSIGNAL

namespace ooe
{
	namespace platform
	{
		class socket;
	}

	class platform::socket
		: public ooe::descriptor
	{
	protected:
		socket( const descriptor& );
	};
}

#endif	// OOE_FOUNDATION_IO_SOCKET_FORWARD_HPP
