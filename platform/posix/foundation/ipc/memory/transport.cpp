/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/environment.hpp"
#include "foundation/io/socket.hpp"
#include "foundation/ipc/memory/transport.hpp"
#include "foundation/utility/miscellany.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe )( ipc ) )

//--- cast -----------------------------------------------------------------------------------------
inline shared_memory::type cast( bool create )
{
    return create ? shared_memory::create : shared_memory::open;
}

OOE_ANONYMOUS_END( ( ooe )( ipc ) )

OOE_NAMESPACE_BEGIN( ( ooe )( platform )( ipc )( memory ) )

//--- transport ------------------------------------------------------------------------------------
transport::transport( bool created_ )
    : created( created_ ), in(), out()
{
}

OOE_STATIC_ASSERT( executable::static_page_size >
    sizeof( unnamed_semaphore ) * 2 + ooe::ipc::memory::transport::private_size );

OOE_NAMESPACE_END( ( ooe )( platform )( ipc )( memory ) )

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

//--- transport ------------------------------------------------------------------------------------
transport::transport( const std::string& name_, type mode )
    : platform::ipc::memory::transport( mode == create ),
    memory( name_, cast( mode == create ), executable::static_page_size * 2 )
{
    ooe::memory::region window( executable::static_page_size, executable::static_page_size );
    memory.protect( ooe::memory::none, window );

    platform::ipc::unnamed_semaphore* pointer =
        add< platform::ipc::unnamed_semaphore >( memory.get(), private_size );

    if ( created )
    {
        in = new( pointer + 0 ) platform::ipc::unnamed_semaphore( 0 );
        out = new( pointer + 1 ) platform::ipc::unnamed_semaphore( 0 );
    }
    else
    {
        in = pointer + 0;
        out = pointer + 1;
    }
}

transport::transport( ooe::socket& socket )
    : platform::ipc::memory::transport( shared_memory::open ),
    memory( std::string(), socket.receive() )
{
    ooe::memory::region window( executable::static_page_size, executable::static_page_size );
    memory.protect( ooe::memory::none, window );

    platform::ipc::unnamed_semaphore* pointer =
        add< platform::ipc::unnamed_semaphore >( memory.get(), private_size );
    in = pointer + 0;
    out = pointer + 1;
}

transport::~transport( void )
{
    if ( created )
    {
        out->~unnamed_semaphore();
        in->~unnamed_semaphore();
    }
}

void transport::wait( wait_type function, const void* pointer )
{
    in->down();
    function( pointer );
    out->up();
}

void transport::notify( void )
{
    in->up();
    out->down();
}

void transport::wake_wait( void )
{
    in->up();
}

void transport::wake_notify( void )
{
    out->up();
}

u8* transport::get( void ) const
{
    return memory.as< u8 >() + sizeof( platform::ipc::unnamed_semaphore ) * 2 + private_size;
}

up_t transport::size( void ) const
{
    return memory.size() - sizeof( platform::ipc::unnamed_semaphore ) * 2 - private_size -
        executable::static_page_size;
}

void transport::migrate( ooe::socket& socket )
{
    socket.send( memory );
    created = false;
}

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )
