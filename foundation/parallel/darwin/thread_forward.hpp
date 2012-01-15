/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_PARALLEL_DARWIN_THREAD_FORWARD_HPP
#define OOE_FOUNDATION_PARALLEL_DARWIN_THREAD_FORWARD_HPP

#include "foundation/utility/string.hpp"

#define OOE_TLS( type ) ooe::tls< type >

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

void thread_name( const std::string& );

OOE_NAMESPACE_END( ( ooe )( platform ) )

#endif  // OOE_FOUNDATION_PARALLEL_DARWIN_THREAD_FORWARD_HPP
