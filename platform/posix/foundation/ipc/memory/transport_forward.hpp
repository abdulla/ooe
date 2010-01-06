/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_FORWARD_HPP
#define OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_FORWARD_HPP

#include "foundation/ipc/semaphore.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( platform )( ipc )( memory ) )

class transport
{
protected:
	bool create_;
	unnamed_semaphore* in;
	unnamed_semaphore* out;

	transport( bool );
};

OOE_NAMESPACE_END( ( ooe )( platform )( ipc )( memory ) )

#endif	// OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_FORWARD_HPP
