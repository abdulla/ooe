/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_PLATFORM_DARWIN_FOUNDATION_PARALLEL_THREAD_FORWARD_HPP
#define OOE_PLATFORM_DARWIN_FOUNDATION_PARALLEL_THREAD_FORWARD_HPP

#include "foundation/utility/string.hpp"

#define OOE_TLS( type ) ooe::tls< type >

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

void thread_name( const std::string& );

OOE_NAMESPACE_END( ( ooe )( platform ) )

#endif  // OOE_PLATFORM_DARWIN_FOUNDATION_PARALLEL_THREAD_FORWARD_HPP
