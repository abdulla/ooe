/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <cerrno>

#include <fcntl.h>

#include "foundation/ipc/error.hpp"
#include "foundation/ipc/semaphore.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe )( ipc ) )

//--- sem_remove -----------------------------------------------------------------------------------
void sem_remove( const std::string& name )
{
    if ( sem_unlink( name.c_str() ) && errno != OOE_NOEXIST )
        throw error::runtime( "ipc::semaphore: " ) <<
            "Unable to unlink semaphore \"" << name << "\": " << error::number( errno );
}

//--- sem_create -----------------------------------------------------------------------------------
sem_t* sem_create( const std::string& name, bool create, u32 value )
{
    if ( create )
        sem_remove( name );

    s32 flags = create ? O_CREAT | O_EXCL: 0;
    sem_t* sem = sem_open( name.c_str(), flags, 0600, value );

    if ( sem == SEM_FAILED )
        throw error::runtime( "ipc::semaphore: " ) <<
            "Unable to open semaphore \"" << name << "\": " << error::number( errno );

    return sem;
}

//--- sem_destroy ----------------------------------------------------------------------------------
void sem_destroy( const std::string& name, bool unlinkable, sem_t* sem )
{
    if ( sem_close( sem ) )
        OOE_CONSOLE( "ipc::semaphore: " <<
            "Unable to close semaphore \"" << name << "\": " << error::number( errno ) );

    if ( unlinkable && sem_unlink( name.c_str() ) && errno != OOE_NOEXIST )
        OOE_CONSOLE( "ipc::semaphore: " <<
            "Unable to unlink semaphore \"" << name << "\": " << error::number( errno ) );
}

OOE_ANONYMOUS_END( ( ooe )( ipc ) )

OOE_NAMESPACE_BEGIN( ( ooe )( ipc ) )

//--- semaphore ------------------------------------------------------------------------------------
semaphore::semaphore( const std::string& sem_name, type mode, u32 value )
    : name_( sem_name ), unlinkable( mode == create ),
    sem( sem_create( name_, mode == create, value ) )
{
}

semaphore::~semaphore( void )
{
    sem_destroy( name_, unlinkable, sem );
}

void semaphore::up( void )
{
    if ( sem_post( sem ) )
        throw error::runtime( "ipc::semaphore: " ) <<
            "Unable to increment semaphore: " << error::number( errno );
}

void semaphore::down( void )
{
    if ( sem_wait( sem ) )
        throw error::runtime( "ipc::semaphore: " ) <<
            "Unable to decrement semaphore: " << error::number( errno );
}

std::string semaphore::name( void ) const
{
    return name_;
}

void semaphore::unlink( void )
{
    if ( unlinkable )
    {
        sem_remove( name_ );
        name_ = std::string();
        unlinkable = false;
    }
}

void semaphore::set( bool value )
{
    unlinkable = value;
}

//--- process_lock ---------------------------------------------------------------------------------
process_lock::process_lock( ipc::semaphore& semaphore_ )
    : semaphore( semaphore_ )
{
    semaphore.down();
}

process_lock::~process_lock( void )
{
    semaphore.up();
}

//--- barrier_wait ---------------------------------------------------------------------------------
barrier_wait::barrier_wait( const std::string& name )
    : semaphore( name, semaphore::create, 0 )
{
}

barrier_wait::~barrier_wait( void )
{
    semaphore.down();
}

//--- barrier_notify -------------------------------------------------------------------------------
void barrier_notify( const std::string& name )
{
    semaphore( name ).up();
}

OOE_NAMESPACE_END( ( ooe )( ipc ) )
