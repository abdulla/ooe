/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/memory/transport.hpp"


namespace ooe
{
//--- ipc::memory::transport ---------------------------------------------------
	void* ipc::memory::transport::private_data( void ) const
	{
		return get() + size();
	}

	std::string ipc::memory::transport::name( void ) const
	{
		return memory.name();
	}
}
