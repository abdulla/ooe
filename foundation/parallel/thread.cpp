/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <cerrno>

#include "foundation/parallel/thread.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

void* startup( void* pointer )
{
    thread::tuple_type& tuple = *static_cast< thread::tuple_type* >( pointer );
    platform::thread_name( tuple._0 );
    OOE_PRINT( "thread \"" << tuple._0 << "\"", return tuple._1( tuple._2 ) );
    return 0;
}

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
    : pthread( pthread_self() ), tuple( thread_name( pthread ), function_type(), 0 ), joined( true )
{
}

thread::thread( const std::string& name_, const function_type& function, void* data )
    : pthread(), tuple( name_, function, data ), joined( false )
{
    s32 status = pthread_create( &pthread, 0, startup, &tuple );

    if ( status )
        throw error::runtime( "thread \"" ) << tuple._0 << "\": "
            "Unable to create thread: " << error::number( status );
}

thread::~thread( void )
{
    if ( joined )
        return;

    s32 status = pthread_detach( pthread );

    if ( status && status != ESRCH )
        OOE_CONSOLE( "thread \"" << tuple._0 << "\": "
            "Unable to destroy thread: " << error::number( status ) );
}

std::string thread::name( void ) const
{
    return tuple._0;
}

bool thread::operator ==( const thread& compare ) const
{
    return pthread_equal( pthread, compare.pthread );
}

void* thread::join( void )
{
    if ( joined )
        return 0;

    void* pointer = 0;
    s32 status = pthread_join( pthread, &pointer );

    if ( status && status != ESRCH )
        throw error::runtime( "thread \"" ) << tuple._0 << "\": "
            "Unable to join thread: " << error::number( status );

    joined = true;
    return pointer;
}

void thread::exit( void* pointer )
{
    pthread_exit( pointer );
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
