/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_FORWARD_HPP
#define OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_FORWARD_HPP

#include "foundation/ipc/semaphore.hpp"

namespace ooe
{
	struct socket;

	namespace platform
	{
		namespace ipc
		{
			namespace memory
			{
				class transport;
			}
		}
	}

	class platform::ipc::memory::transport
	{
	protected:
		ooe::ipc::semaphore in;
		ooe::ipc::semaphore out;

		transport( const std::string&, u8 );
		transport( ooe::socket& );
	};
}

#endif	// OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_FORWARD_HPP
