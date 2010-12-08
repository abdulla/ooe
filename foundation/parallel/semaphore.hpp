/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_PARALLEL_SEMAPHORE_HPP
#define OOE_FOUNDATION_PARALLEL_SEMAPHORE_HPP

#include "foundation/parallel/semaphore_forward.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

struct OOE_VISIBLE semaphore
    : private platform::semaphore
{
    semaphore( u32 );
    ~semaphore( void );

    void up( void );
    void down( void );
};

class OOE_VISIBLE semaphore_lock
{
public:
    semaphore_lock( ooe::semaphore& );
    ~semaphore_lock( void );

private:
    ooe::semaphore& semaphore;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_PARALLEL_SEMAPHORE_HPP
