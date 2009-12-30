/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_NAME_HPP
#define OOE_FOUNDATION_IPC_NAME_HPP

#include "foundation/ipc/fundamental.hpp"
#include "foundation/utility/macro.hpp"
#include "foundation/utility/string.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc ) )

std::string link_name( pid_t, time_t, link_t );
std::string local_name( const std::string& );
std::string unique_name( void ) OOE_VISIBLE;

OOE_NAMESPACE_END( ( ooe )( ipc ) )

#endif	// OOE_FOUNDATION_IPC_NAME_HPP
