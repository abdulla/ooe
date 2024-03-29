/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <tbb/concurrent_queue.h>

#include "foundation/executable/environment.hpp"
#include "foundation/parallel/thread.hpp"
#include "foundation/parallel/thread_pool.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- thread_unit ----------------------------------------------------------------------------------
class thread_unit
{
public:
    thread_unit( const std::string& name, thread_pool& pool )
        : state( true ), loads( 0 ), semaphore( 0 ), queue(),
        thread( name, make_function( *this, &thread_unit::main ), &pool )
    {
    }

    ~thread_unit( void )
    {
        state.exchange( false );
        semaphore.up();
    }

    void enqueue( const task_ptr& task )
    {
        queue.push( task );

        if ( !loads++ )
            semaphore.up();
    }

    bool dequeue( task_ptr& task )
    {
        if ( !loads || !queue.try_pop( task ) )
            return false;

        --loads;
        return true;
    }

private:
    atom< bool > state;
    atom< up_t > loads;

    ooe::semaphore semaphore;
    tbb::concurrent_queue< task_ptr > queue;
    ooe::thread thread;

    void main( void* /*pointer*/ )
    {
        //thread_pool& pool = *static_cast< thread_pool* >( pointer );

        while ( state )
        {
            for ( task_ptr task; dequeue( task ) /*|| pool.snoop( task )*/; )
            {
                task->state.exchange( task_base::error );
                OOE_PRINT( "thread_pool \"" << thread.name() << "\"",
                    ( *task )(); task->state.exchange( task_base::done ) );

                if ( !task.unique() )
                    task->semaphore.up();
            }

            if ( !loads )
                semaphore.down();
        }
    }
};

//--- thread_pool ----------------------------------------------------------------------------------
thread_pool::thread_pool( const std::string& name )
    : index( 0 ), vector()
{
    up_t cpu_cores = executable::cpu_cores();
    vector.reserve( cpu_cores );

    for ( up_t i = 0; i != cpu_cores; ++i )
    {
        std::string unit_name = name + ' ';
        vector.push_back( opaque_ptr( new thread_unit( unit_name << i, *this ) ) );
    }
}

void thread_pool::insert( const task_ptr& task )
{
    vector[ index ].as< thread_unit >()->enqueue( task );
    ++index %= vector.size();
}

bool thread_pool::snoop( task_ptr& task )
{
    // start at last index, which would have a high probability of containing a task
    up_t start = index - 1;

    for ( up_t size = vector.size(), j = size; j; --j )
    {
        up_t i = ( start + j ) % size;

        if ( vector[ i ].as< thread_unit >()->dequeue( task ) )
            return true;
    }

    return false;
}

OOE_NAMESPACE_END( ( ooe ) )
