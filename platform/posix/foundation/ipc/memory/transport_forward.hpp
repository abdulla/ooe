/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_FORWARD_HPP
#define OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_FORWARD_HPP

#include "foundation/ipc/semaphore.hpp"

namespace ooe
{
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
		u8 mode;
		unnamed_semaphore* in;
		unnamed_semaphore* out;

		transport( u8 mode );
	};
}

#endif	// OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_FORWARD_HPP
