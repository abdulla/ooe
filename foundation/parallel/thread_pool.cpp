/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/executable/environment.hpp"
#include "foundation/parallel/queue.hpp"
#include "foundation/parallel/semaphore.hpp"
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

    void stop( void )
    {
        state = false;
        semaphore.up();
        thread.join();
    }

    void enqueue( const task_ptr& task )
    {
        queue.enqueue( task );

        if ( !loads++ )
            semaphore.up();
    }

    bool dequeue( task_ptr& task )
    {
        if ( !loads || !queue.dequeue( task ) )
            return false;

        --loads;
        return true;
    }

private:
    atom< bool > state;
    atom< up_t > loads;

    ooe::semaphore semaphore;
    ooe::queue< task_ptr > queue;
    ooe::thread thread;

    void* main( void* pointer )
    {
        thread_pool& pool = *static_cast< thread_pool* >( pointer );

        while ( state )
        {
            for ( task_ptr task; dequeue( task ) || pool.snoop( task ); )
            {
                task->state = task_base::error;
                OOE_PRINT( "thread_pool \"" << thread.name() << "\"",
                    ( *task )(); task->state = task_base::done );

                if ( !task.unique() )
                    task->semaphore.up();
            }

            if ( !loads )
                semaphore.down();
        }

        return 0;
    }
};

//--- thread_pool ----------------------------------------------------------------------------------
thread_pool::thread_pool( const std::string& name )
    : index( 0 ), vector()
{
    for ( up_t i = 0, end = executable::cpu_cores(); i != end; ++i )
    {
        std::string unit_name = name + ' ';
        vector.push_back( new thread_unit( unit_name << i, *this ) );
    }
}

thread_pool::~thread_pool( void )
{
    for ( vector_type::iterator i = vector.begin(), end = vector.end(); i != end; ++i )
        i->as< thread_unit >()->stop();
}

void thread_pool::insert( const task_ptr& task )
{
    vector[ index ].as< thread_unit >()->enqueue( task );
    ++index %= vector.size();
}

bool thread_pool::snoop( task_ptr& task )
{
    // TODO: start at index - 1, and iterate backwards
    for ( vector_type::iterator i = vector.begin(), end = vector.end(); i != end; ++i )
    {
        if ( i->as< thread_unit >()->dequeue( task ) )
            return true;
    }

    return false;
}

OOE_NAMESPACE_END( ( ooe ) )
