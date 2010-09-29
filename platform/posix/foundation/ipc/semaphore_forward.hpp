/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_PLATFORM_POSIX_FOUNDATION_IPC_SEMAPHORE_FORWARD_HPP
#define OOE_PLATFORM_POSIX_FOUNDATION_IPC_SEMAPHORE_FORWARD_HPP

#include <semaphore.h>

#include "foundation/utility/fundamental.hpp"
#include "foundation/utility/macro.hpp"

namespace ooe
{
    namespace platform
    {
        namespace ipc
        {
            class unnamed_semaphore;
        }
    }

//--- platform::ipc::unnamed_semaphore -----------------------------------------
    class platform::ipc::unnamed_semaphore
    {
    public:
        unnamed_semaphore( u32 );
        ~unnamed_semaphore( void );

        void up( void );
        void down( void );

    private:
        sem_t sem;
    };
}

#endif  // OOE_PLATFORM_POSIX_FOUNDATION_IPC_SEMAPHORE_FORWARD_HPP
