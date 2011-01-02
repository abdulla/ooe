/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_PRIVATE_HPP
#define OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_PRIVATE_HPP

#include "foundation/ipc/memory/transport.hpp"
#include "foundation/parallel/thread.hpp"
#include "foundation/utility/atom.hpp"
#include "foundation/utility/error.hpp"

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
        wake_notify( out );
    }

    template< typename t >
        void notify( t& in, t& out )
    {
        wake_wait( in );

        for ( u8 i = 1 << 4; i && lock == locked; --i )
            thread::yield();

        if ( lock.compare_exchange( locked, sleeping ) == locked )
            out.down();
    }

    template< typename t >
        void wake_wait( t& in )
    {
        if ( lock.exchange( locked ) != unlocked )
            throw error::runtime( "control: " ) << "Invalid lock state";

        in.up();
    }

    template< typename t >
        void wake_notify( t& out )
    {
        if ( lock.compare_exchange( locked, unlocked ) == locked )
            thread::yield();
        else if ( lock.compare_exchange( sleeping, unlocked ) == sleeping )
            out.up();
        else
            throw error::runtime( "control: " ) << "Invalid lock state";
    }
};

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )

#endif  // OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_PRIVATE_HPP
