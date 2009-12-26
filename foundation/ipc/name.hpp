/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_NAME_HPP
#define OOE_FOUNDATION_IPC_NAME_HPP

#include "foundation/utility/macro.hpp"
#include "foundation/utility/string.hpp"

namespace ooe
{
	namespace ipc
	{
		std::string link_name( pid_t, time_t, u32 );
		std::string local_name( const std::string& );
		std::string unique_name( void ) OOE_VISIBLE;
	}
}

#endif	// OOE_FOUNDATION_IPC_NAME_HPP
