/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/environment.hpp"
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
transport::transport( const std::string& name_ )
    : platform::ipc::memory::transport( true ),
    memory( name_, shared_memory::create, executable::static_page_size * 2 )
{
    ooe::memory::region region( executable::static_page_size, executable::static_page_size );
    memory.protect( ooe::memory::none, region );

    new( memory.get() ) control_io;
}

transport::transport( ooe::socket& socket )
    : platform::ipc::memory::transport( false ), memory( socket.receive() )
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
    control_io& control = *memory.as< control_io >();
    control.wake_wait( control.in );
}

void transport::wake_notify( void )
{
    control_io& control = *memory.as< control_io >();
    control.wake_notify( control.out );
}

u8* transport::get( void ) const
{
    return memory.as< u8 >() + sizeof( control_io );
}

up_t transport::size( void ) const
{
    return memory.size() - sizeof( control_io ) - executable::static_page_size;
}

void transport::send( ooe::socket& socket )
{
    socket.send( memory );
}

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )
