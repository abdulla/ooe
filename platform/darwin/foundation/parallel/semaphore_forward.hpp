/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_PLATFORM_POSIX_FOUNDATION_PARALLEL_SEMAPHORE_FORWARD_HPP
#define OOE_PLATFORM_POSIX_FOUNDATION_PARALLEL_SEMAPHORE_FORWARD_HPP

#include <mach/semaphore.h>

#include "foundation/utility/fundamental.hpp"
#include "foundation/utility/macro.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

class semaphore
{
protected:
    semaphore_t sem;

    semaphore( u32 );
    ~semaphore( void ) OOE_VISIBLE;
};

OOE_NAMESPACE_END( ( ooe )( platform ) )

#endif  // OOE_PLATFORM_POSIX_FOUNDATION_PARALLEL_SEMAPHORE_FORWARD_HPP
