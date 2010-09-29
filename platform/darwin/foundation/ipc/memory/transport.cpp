/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/environment.hpp"
#include "foundation/io/socket.hpp"
#include "foundation/ipc/memory/transport.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe )( ipc ) )

//--- cast_sem -------------------------------------------------------------------------------------
inline semaphore::type cast_sem( bool create )
{
    return create ? semaphore::create : semaphore::open;
}

//--- cast_shm -------------------------------------------------------------------------------------
inline shared_memory::type cast_shm( bool create )
{
    return create ? shared_memory::create : shared_memory::open;
}

//--- receive_name ---------------------------------------------------------------------------------
inline std::string receive_name( socket& socket )
{
    u32 size;

    if ( socket.receive( &size, sizeof( size ) ) != sizeof( size ) )
        throw error::runtime( "ipc::memory::transport: " ) << "Unable to receive size";

    std::string name( size, 0 );

    if ( socket.receive( &name[ 0 ], size ) != size )
        throw error::runtime( "ipc::memroy::transport: " ) << "Unable to receive name";

    return name;
}

//--- send_name ------------------------------------------------------------------------------------
inline void send_name( socket& socket, const std::string& name )
{
    u32 size = name.size();

    if ( socket.send( &size, sizeof( size ) ) != sizeof( size ) )
        throw error::runtime( "ipc::memory::transport: " ) << "Unable to send size";
    else if ( socket.send( name.c_str(), size ) != size )
        throw error::runtime( "ipc::memory::transport: " ) << "Unable to send name";
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe )( ipc ) )

OOE_NAMESPACE_BEGIN( ( ooe )( platform )( ipc )( memory ) )

//--- transport ------------------------------------------------------------------------------------
transport::transport( const std::string& name_, bool create )
    : in( name_ + ".i", cast_sem( create ), 0 ), out( name_ + ".o", cast_sem( create ), 0 )
{
}

transport::transport( ooe::socket& socket )
    : in( receive_name( socket ) ), out( receive_name( socket ) )
{
}

OOE_STATIC_ASSERT( executable::static_page_size > ooe::ipc::memory::transport::private_size );

OOE_NAMESPACE_END( ( ooe )( platform )( ipc )( memory ) )

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

//--- transport ------------------------------------------------------------------------------------
transport::transport( const std::string& name_, type mode )
    : platform::ipc::memory::transport( name_, mode == create ),
    memory( name_, cast_shm( mode ), executable::static_page_size * 2 )
{
    ooe::memory::region window( executable::static_page_size, executable::static_page_size );
    memory.protect( ooe::memory::none, window );
}

transport::transport( ooe::socket& socket )
    : platform::ipc::memory::transport( socket ), memory( std::string(), socket.receive() )
{
    ooe::memory::region window( executable::static_page_size, executable::static_page_size );
    memory.protect( ooe::memory::none, window );

    in.unlinkable = true;
    out.unlinkable = true;
}

transport::~transport( void )
{
}

void transport::wait( wait_type function, const void* pointer )
{
    in.down();
    function( pointer );
    out.up();
}

void transport::notify( void )
{
    in.up();
    out.down();
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
    return memory.as< u8 >() + private_size;
}

up_t transport::size( void ) const
{
    return memory.size() - private_size - executable::static_page_size;
}

void transport::migrate( ooe::socket& socket )
{
    send_name( socket, in.name() );
    send_name( socket, out.name() );
    socket.send( memory );

    in.unlinkable = false;
    out.unlinkable = false;
}

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )
