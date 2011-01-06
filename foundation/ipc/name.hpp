/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_NAME_HPP
#define OOE_FOUNDATION_IPC_NAME_HPP

#include "foundation/utility/macro.hpp"
#include "foundation/utility/string.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc ) )

std::string local_name( const std::string& ) OOE_VISIBLE;
std::string server_name( const std::string& ) OOE_VISIBLE;
std::string unique_name( void ) OOE_VISIBLE;

OOE_NAMESPACE_END( ( ooe )( ipc ) )

#endif  // OOE_FOUNDATION_IPC_NAME_HPP
