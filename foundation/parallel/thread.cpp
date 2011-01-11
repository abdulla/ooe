/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <cerrno>

#include "foundation/parallel/thread.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

typedef tuple< std::string, thread::function_type, void* > datum_tuple;
typedef scoped_ptr< datum_tuple > datum_ptr;

std::string thread_name( pthread_t pthread )
{
    c8 name[ 256 ];
    s32 status = pthread_getname_np( pthread, name, sizeof( name ) );

    if ( status )
    {
        OOE_CONSOLE( "thread: Unable to get name: " << error::number( errno ) );
        return std::string();
    }

    return name;
}

void* startup( void* pointer )
{
    datum_ptr datum( static_cast< datum_tuple* >( pointer ) );
    platform::thread_name( datum->_0 );
    datum->_0 = std::string();

    OOE_PRINT( "thread \"" << thread_name( pthread_self() ) << "\"", datum->_1( datum->_2 ) );
    return 0;
}

void specify( pthread_key_t key, const void* value )
{
    s32 status = pthread_setspecific( key, value );

    if ( status )
        throw error::runtime( "tls: " ) << "Unable to set value: " << error::number( status );
}

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- thread ---------------------------------------------------------------------------------------
thread::thread( void )
    : pthread( pthread_self() ), detached( true )
{
}

thread::thread( const std::string& name_, const function_type& function, void* data )
    : pthread(), detached( false )
{
    datum_ptr datum( new datum_tuple( name_, function, data ) );
    s32 status = pthread_create( &pthread, 0, startup, datum );

    if ( status )
        throw error::runtime( "thread \"" ) << name() << "\": "
            "Unable to create thread: " << error::number( status );

    datum.release();
}

thread::~thread( void )
{
    if ( detached )
        return;

    s32 status = pthread_join( pthread, 0 );

    if ( status )
        OOE_CONSOLE( "thread \"" << name() << "\": "
            "Unable to join thread: " << error::number( status ) );
}

bool thread::operator ==( const thread& compare ) const
{
    return pthread_equal( pthread, compare.pthread );
}

std::string thread::name( void ) const
{
    return thread_name( pthread );
}

bool thread::is_detached( void ) const
{
    return detached;
}

void thread::detach( void )
{
    if ( detached )
        return;

    s32 status = pthread_detach( pthread );

    if ( status )
        throw error::runtime( "thread \"" ) << name() << "\": "
            "Unable to detach thread: " << error::number( status );

    detached = true;
}

void thread::exit( void )
{
    pthread_exit( 0 );
}

//--- tls_base -------------------------------------------------------------------------------------
tls_base::tls_base( create_type create_, destroy_type destroy )
    : pthread_key(), create( create_ )
{
    s32 status = pthread_key_create( &pthread_key, destroy );

    if ( status )
        throw error::runtime( "tls: " ) << "Unable to create key: " << error::number( status );
}

tls_base::~tls_base( void )
{
    s32 status = pthread_key_delete( pthread_key );

    if ( status )
        OOE_CONSOLE( "tls: " << "Unable to destroy key: " << error::number( status ) );
}

void* tls_base::get( void ) const
{
    void* value = pthread_getspecific( pthread_key );

    if ( create && !value )
    {
        value = create();
        specify( pthread_key, value );
    }

    return value;
}

void tls_base::set( const void* value )
{
    specify( pthread_key, value );
}

OOE_NAMESPACE_END( ( ooe ) )
