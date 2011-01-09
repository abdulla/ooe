/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_NAME_HPP
#define OOE_FOUNDATION_IPC_NAME_HPP

#include "foundation/io/socket.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc ) )

std::string unique_name( void ) OOE_VISIBLE;
std::string local_name( const std::string& ) OOE_VISIBLE;
local_address server_address( const std::string& ) OOE_VISIBLE;

OOE_NAMESPACE_END( ( ooe )( ipc ) )

#endif  // OOE_FOUNDATION_IPC_NAME_HPP
