/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IO_SOCKET_FORWARD_HPP
#define OOE_FOUNDATION_IO_SOCKET_FORWARD_HPP

#include "foundation/io/descriptor.hpp"

#define OOE_SOCKET_SEND_FLAG 0

namespace ooe
{
	namespace platform
	{
		class socket;
	}

	class platform::socket
		: private ooe::descriptor
	{
	protected:
		socket( const descriptor& );

		using descriptor::get;
	};
}

#endif	// OOE_FOUNDATION_IO_SOCKET_FORWARD_HPP
