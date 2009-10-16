/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

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
		const u8 mode;
		ooe::ipc::unnamed_semaphore* in;
		ooe::ipc::unnamed_semaphore* out;

		transport( u8 mode );
	};
}
#define OOE_NOEXIST ENOENT

#endif	// OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_FORWARD_HPP
