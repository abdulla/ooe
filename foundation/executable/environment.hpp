/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_EXECUTABLE_ENVIRONMENT_HPP
#define OOE_FOUNDATION_EXECUTABLE_ENVIRONMENT_HPP

#include "foundation/utility/macro.hpp"
#include "foundation/utility/preprocessor.hpp"

#include BOOST_PP_STRINGIZE( foundation/executable/OOE_PLATFORM/environment_forward.hpp )

OOE_NAMESPACE_BEGIN( ( ooe )( executable ) )

void yield( void ) OOE_VISIBLE;
up_t cpu_cores( void ) OOE_VISIBLE;
up_t page_size( void ) OOE_VISIBLE;

OOE_NAMESPACE_END( ( ooe )( executable ) )

#endif  // OOE_FOUNDATION_EXECUTABLE_ENVIRONMENT_HPP
