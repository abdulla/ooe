/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/parallel/lock.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/utility/scoped.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

#ifdef __OPTIMIZE__
const ooe::s32 mutex_type = PTHREAD_MUTEX_NORMAL;
#else
const ooe::s32 mutex_type = PTHREAD_MUTEX_ERRORCHECK;
#endif

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- mutex ----------------------------------------------------------------------------------------
mutex::mutex( void )
    : pthread_mutex()
{
    pthread_mutexattr_t attributes;
    s32 status = pthread_mutexattr_init( &attributes );

    if ( status )
        throw error::runtime( "mutex: " ) <<
            "Unable to create mutex attributes: " << error::number( status );

    scoped< int ( pthread_mutexattr_t* ) > scoped( pthread_mutexattr_destroy, &attributes );
    status = pthread_mutexattr_settype( &attributes, mutex_type );

    if ( status )
        throw error::runtime( "mutex: " ) <<
            "Unable to set mutex type: " << error::number( status );

    status = pthread_mutex_init( &pthread_mutex, &attributes );

    if ( status )
        throw error::runtime( "mutex: " ) << "Unable to create mutex: " << error::number( status );
}

mutex::~mutex( void )
{
    s32 status = pthread_mutex_destroy( &pthread_mutex );

    if ( status )
        OOE_CONSOLE( "mutex: " << "Unable to destroy mutex: " << error::number( status ) );
}

//--- condition ------------------------------------------------------------------------------------
condition::condition( void )
    : pthread_cond()
{
    s32 status = pthread_cond_init( &pthread_cond, 0 );

    if ( status )
        throw error::runtime( "condition: " ) <<
            "Unable to create condition: " << error::number( status );
}

condition::~condition( void )
{
    s32 status = pthread_cond_destroy( &pthread_cond );

    if ( status )
        OOE_CONSOLE( "condition: " << "Unable to destroy condition: " << error::number( status ) );
}

void condition::wait( lock& lock )
{
    s32 status = pthread_cond_wait( &pthread_cond, &lock.mutex.pthread_mutex );

    if ( status )
        throw error::runtime( "condition: " ) <<
            "Unable to wait on condition: " << error::number( status );
}

void condition::notify_one( void )
{
    s32 status = pthread_cond_signal( &pthread_cond );

    if ( status )
        throw error::runtime( "condition: " ) <<
            "Unable to signal condition: " << error::number( status );
}

void condition::notify_all( void )
{
    s32 status = pthread_cond_broadcast( &pthread_cond );

    if ( status )
        throw error::runtime( "condition: " ) <<
            "Unable to broadcast condition: " << error::number( status );
}

//--- read_write -----------------------------------------------------------------------------------
read_write::read_write( void )
    : pthread_rwlock()
{
    s32 status = pthread_rwlock_init( &pthread_rwlock, 0 );

    if ( status )
        throw error::runtime( "read_write: " ) <<
            "Unable to create read write lock: " << error::number( status );
}

read_write::~read_write( void )
{
    s32 status = pthread_rwlock_destroy( &pthread_rwlock );

    if ( status )
        OOE_CONSOLE( "read_write: " <<
            "Unable to destroy read write lock: " << error::number( status ) );
}

//--- lock -----------------------------------------------------------------------------------------
lock::lock( ooe::mutex& mutex_ )
    : mutex( mutex_ )
{
    s32 status = pthread_mutex_lock( &mutex.pthread_mutex );

    if ( status )
        throw error::runtime( "lock: " ) << "Unable to lock mutex: " << error::number( status );
}

lock::~lock( void )
{
    s32 status = pthread_mutex_unlock( &mutex.pthread_mutex );

    if ( status )
        OOE_CONSOLE( "lock: " << "Unable to unlock mutex: " << error::number( status ) );
}

//--- read_lock ------------------------------------------------------------------------------------
read_lock::read_lock( ooe::read_write& read_write_ )
    : read_write( read_write_ )
{
    s32 status = pthread_rwlock_rdlock( &read_write.pthread_rwlock );

    if ( status )
        throw error::runtime( "read_lock: " ) << "Unable to lock read: " <<
            error::number( status );
}

read_lock::~read_lock( void )
{
    s32 status = pthread_rwlock_unlock( &read_write.pthread_rwlock );

    if ( status )
        OOE_CONSOLE( "read_lock: " << "Unable to unlock read: " << error::number( status ) );
}

//--- write_lock -----------------------------------------------------------------------------------
write_lock::write_lock( ooe::read_write& read_write_ )
    : read_write( read_write_ )
{
    s32 status = pthread_rwlock_wrlock( &read_write.pthread_rwlock );

    if ( status )
        throw error::runtime( "write_lock: " ) << "Unable to lock write: " <<
            error::number( status );
}

write_lock::~write_lock( void )
{
    s32 status = pthread_rwlock_unlock( &read_write.pthread_rwlock );

    if ( status )
        OOE_CONSOLE( "write_lock: " << "Unable to unlock write: " << error::number( status ) );
}

OOE_NAMESPACE_END( ( ooe ) )
