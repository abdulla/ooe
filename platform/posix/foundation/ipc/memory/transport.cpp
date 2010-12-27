/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/environment.hpp"
#include "foundation/io/socket.hpp"
#include "foundation/ipc/memory/transport_private.hpp"
#include "foundation/parallel/lock.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe )( ipc )( memory ) )

//--- control_io -----------------------------------------------------------------------------------
struct control_io
    : public control
{
    ooe::semaphore in;
    ooe::semaphore out;

    control_io( void )
        : control(), in( 0 ), out( 0 )
    {
    }
};

OOE_STATIC_ASSERT( executable::static_page_size > sizeof( control_io ) );

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
        new( memory.get() ) control_io;
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
        memory.as< control_io >()->~control_io();
}

void transport::wait( wait_type function, const void* pointer )
{
    control_io& control = *memory.as< control_io >();
    control.wait( control.in, control.out, function, pointer );
}

void transport::notify( void )
{
    control_io& control = *memory.as< control_io >();
    control.notify( control.in, control.out );
}

void transport::wake_wait( void )
{
    memory.as< control_io >()->in.up();
}

void transport::wake_notify( void )
{
    memory.as< control_io >()->out.up();
}

u8* transport::get( void ) const
{
    return memory.as< u8 >() + sizeof( control_io );
}

up_t transport::size( void ) const
{
    return memory.size() - sizeof( control_io ) - executable::static_page_size;
}

void transport::migrate( ooe::socket& socket )
{
    socket.send( memory );
    created = false;
}

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )
