/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/environment.hpp"
#include "foundation/io/socket.hpp"
#include "foundation/ipc/memory/transport_private.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

OOE_STATIC_ASSERT( executable::static_page_size > sizeof( ipc::memory::control ) );

//--- receive_name ---------------------------------------------------------------------------------
inline std::string receive_name( socket& socket )
{
    u32 size;

    if ( socket.receive( &size, sizeof( size ) ) != sizeof( size ) )
        throw error::runtime( "ipc::memory::transport: " ) << "Unable to receive size";

    std::string name( size, 0 );

    if ( socket.receive( &name[ 0 ], size ) != size )
        throw error::runtime( "ipc::memory::transport: " ) << "Unable to receive name";

    return name;
}

//--- send_name ------------------------------------------------------------------------------------
inline void send_name( socket& socket, const std::string& name )
{
    u32 size = name.size();

    if ( socket.send( &size, sizeof( size ) ) != sizeof( size ) )
        throw error::runtime( "ipc::memory::transport: " ) << "Unable to send size";
    else if ( socket.send( name.data(), size ) != size )
        throw error::runtime( "ipc::memory::transport: " ) << "Unable to send name";
}

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( platform )( ipc )( memory ) )

//--- transport ------------------------------------------------------------------------------------
transport::transport( const std::string& name_, bool create )
    : in( name_ + ".i", ooe::ipc::semaphore::type( create ), 0 ),
    out( name_ + ".o", ooe::ipc::semaphore::type( create ), 0 )
{
}

transport::transport( ooe::socket& socket )
    : in( receive_name( socket ) ), out( receive_name( socket ) )
{
}

OOE_NAMESPACE_END( ( ooe )( platform )( ipc )( memory ) )

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

//--- transport ------------------------------------------------------------------------------------
transport::transport( const std::string& name_, type mode )
    : platform::ipc::memory::transport( name_, mode == create ),
    memory( name_, shared_memory::type( mode ), executable::static_page_size * 2 )
{
    ooe::memory::region region( executable::static_page_size, executable::static_page_size );
    memory.protect( ooe::memory::none, region );

    if ( mode == create )
        new( memory.get() ) control;
}

transport::transport( ooe::socket& socket )
    : platform::ipc::memory::transport( socket ), memory( std::string(), socket.receive() )
{
    ooe::memory::region region( executable::static_page_size, executable::static_page_size );
    memory.protect( ooe::memory::none, region );

    in.set( true );
    out.set( true );
}

transport::~transport( void )
{
}

void transport::wait( wait_type function, const void* pointer )
{
    memory::control& control = *memory.as< memory::control >();
    control.wait( in, out, function, pointer );
}

void transport::notify( void )
{
    memory::control& control = *memory.as< memory::control >();
    control.notify( in, out );
}

void transport::wake_wait( void )
{
    in.up();
}

void transport::wake_notify( void )
{
    out.up();
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
    send_name( socket, in.name() );
    send_name( socket, out.name() );
    socket.send( memory );

    in.set( false );
    out.set( false );
}

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )
