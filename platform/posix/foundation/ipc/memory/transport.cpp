/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/environment.hpp"
#include "foundation/io/socket.hpp"
#include "foundation/ipc/memory/transport.hpp"
#include "foundation/ipc/memory/transport_private.hpp"
#include "foundation/parallel/lock.hpp"
#include "foundation/parallel/thread.hpp"
#include "foundation/utility/atom.hpp"
#include "foundation/utility/miscellany.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe )( ipc )( memory ) )

//--- control --------------------------------------------------------------------------------------
struct control
{
    u8 private_data[ transport::private_size ];
    ooe::semaphore in;
    ooe::semaphore out;
    atom< lock_type > lock;

    control( void )
        : private_data(), in( 0 ), out( 0 ), lock( unlocked )
    {
    }
};

OOE_STATIC_ASSERT( executable::static_page_size > sizeof( control ) );

OOE_ANONYMOUS_END( ( ooe )( ipc )( memory ) )

OOE_NAMESPACE_BEGIN( ( ooe )( platform )( ipc )( memory ) )

//--- transport ------------------------------------------------------------------------------------
transport::transport( bool created_ )
    : created( created_ )
{
}

OOE_NAMESPACE_END( ( ooe )( platform )( ipc )( memory ) )

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

//--- transport ------------------------------------------------------------------------------------
transport::transport( const std::string& name_, type mode )
    : platform::ipc::memory::transport( mode == create ),
    memory( name_, shared_memory::type( mode ), executable::static_page_size * 2 )
{
    ooe::memory::region region( executable::static_page_size, executable::static_page_size );
    memory.protect( ooe::memory::none, region );

    if ( created )
        new( memory.get() ) control;
}

transport::transport( ooe::socket& socket )
    : platform::ipc::memory::transport( true ), memory( std::string(), socket.receive() )
{
    ooe::memory::region region( executable::static_page_size, executable::static_page_size );
    memory.protect( ooe::memory::none, region );
}

transport::~transport( void )
{
    if ( created )
        memory.as< control >()->~control();
}

void transport::wait( wait_type function, const void* pointer )
{
    ::control& control = *memory.as< ::control >();
    control.in.down();
    function( pointer );
    control.lock.cas( locked, unlocked );
    thread::yield();

    if ( control.lock.cas( sleeping, unlocked ) )
        control.out.up();
}

void transport::notify( void )
{
    ::control& control = *memory.as< ::control >();
    control.lock.cas( unlocked, locked );
    control.in.up();

    for ( u8 i = 1 << 4; i && control.lock == locked; --i )
        thread::yield();

    if ( control.lock.cas( locked, sleeping ) )
        control.out.down();
}

void transport::wake_wait( void )
{
    memory.as< control >()->in.up();
}

void transport::wake_notify( void )
{
    memory.as< control >()->out.up();
}

u8* transport::get( void ) const
{
    return memory.as< u8 >() + sizeof( control );
}

up_t transport::size( void ) const
{
    return memory.size() - sizeof( control ) - executable::static_page_size;
}

void transport::migrate( ooe::socket& socket )
{
    socket.send( memory );
    created = false;
}

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )
