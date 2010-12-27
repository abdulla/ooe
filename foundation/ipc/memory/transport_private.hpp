/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_PRIVATE_HPP
#define OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_PRIVATE_HPP

#include "foundation/ipc/memory/transport.hpp"
#include "foundation/parallel/thread.hpp"
#include "foundation/utility/atom.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

struct control
{
    enum lock_type
    {
        unlocked,
        locked,
        sleeping
    };

    u8 private_data[ transport::private_size ];
    atom< lock_type > lock;

    control( void )
        : private_data(), lock( unlocked )
    {
    }

    template< typename t >
        void wait( t& in, t& out, transport::wait_type function, const void* pointer )
    {
        in.down();
        function( pointer );
        lock.cas( locked, unlocked );
        thread::yield();

        if ( lock.cas( sleeping, unlocked ) )
            out.up();
    }

    template< typename t >
        void notify( t& in, t& out )
    {
        lock.cas( unlocked, locked );
        in.up();

        for ( u8 i = 1 << 4; i && lock == locked; --i )
            thread::yield();

        if ( lock.cas( locked, sleeping ) )
            out.down();
    }
};

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )

#endif  // OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_PRIVATE_HPP
