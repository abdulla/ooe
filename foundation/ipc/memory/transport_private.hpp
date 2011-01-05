/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_PRIVATE_HPP
#define OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_PRIVATE_HPP

#include "foundation/ipc/memory/transport.hpp"
#include "foundation/parallel/thread.hpp"
#include "foundation/utility/atom.hpp"
#include "foundation/utility/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

enum lock_type
{
    unlocked,
    locked,
    sleeping
};

typedef atom< lock_type > lock_atom;

template< typename t >
    void spin_up( t& semaphore, lock_atom& spinlock, lock_atom& set )
{
    if ( set.exchange( locked ) != unlocked )
        throw error::runtime( "control: " ) << "Invalid lock state";

    if ( spinlock.compare_exchange( locked, unlocked ) == locked )
        thread::yield();
    else if ( spinlock.compare_exchange( sleeping, unlocked ) == sleeping )
        semaphore.up();
    else
        throw error::runtime( "control: " ) << "Invalid lock state";
}

template< typename t >
    void spin_down( t& semaphore, lock_atom& spinlock )
{
    for ( u8 i = 1 << 4; i && spinlock == locked; --i )
        thread::yield();

    if ( spinlock.compare_exchange( locked, sleeping ) == locked )
        semaphore.down();
}

struct control
{
    lock_atom in_lock;
    lock_atom out_lock;

    control( void )
        : in_lock( locked ), out_lock( unlocked )
    {
    }

    template< typename t >
        void wait( t& in, t& out, transport::wait_type function, const void* pointer )
    {
        spin_down( in, in_lock );
        function( pointer );
        spin_up( out, out_lock, in_lock );
    }

    template< typename t >
        void notify( t& in, t& out )
    {
        spin_up( in, in_lock, out_lock );
        spin_down( out, out_lock );
    }

    template< typename t >
        void wake_wait( t& in )
    {
        spin_up( in, in_lock, out_lock );
    }

    template< typename t >
        void wake_notify( t& out )
    {
        spin_up( out, out_lock, in_lock );
    }
};

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )

#endif  // OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_PRIVATE_HPP
