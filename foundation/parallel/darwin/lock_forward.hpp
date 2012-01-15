/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_PARALLEL_DARWIN_LOCK_FORWARD_HPP
#define OOE_FOUNDATION_PARALLEL_DARWIN_LOCK_FORWARD_HPP

#include <mach/semaphore.h>

#include "foundation/utility/fundamental.hpp"
#include "foundation/utility/macro.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

class semaphore
{
protected:
    semaphore_t sem;

    semaphore( u32 );
    ~semaphore( void );
};

OOE_NAMESPACE_END( ( ooe )( platform ) )

#endif  // OOE_FOUNDATION_PARALLEL_DARWIN_LOCK_FORWARD_HPP
