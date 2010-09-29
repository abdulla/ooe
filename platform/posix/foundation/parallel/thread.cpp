/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/parallel/thread.hpp"

namespace ooe
{
    void thread::yield( void )
    {
        pthread_yield();
    }
}
