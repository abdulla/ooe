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
    thread_unit( const std::string& name )
        : is_waiting( true ), is_running( true ), semaphore( 0 ), queue(),
        thread( name, make_function( *this, &thread_unit::main ), 0 )
    {
    }

    ~thread_unit( void )
    {
        is_running = false;
        semaphore.up();
        thread.join();
    }

    void insert( const task_ptr& task )
    {
        queue.enqueue( task );

        if ( is_waiting )
            semaphore.up();
    }

private:
    atom< bool > is_waiting;
    atom< bool > is_running;

    ooe::semaphore semaphore;
    ooe::queue< task_ptr > queue;
    ooe::thread thread;

    void* main( void* )
    {
        while ( is_running )
        {
            for ( task_ptr task; queue.dequeue( task ); task->semaphore.up() )
            {
                task->state = task_base::error;
                OOE_PRINT( "thread_pool \"" << thread.name() << "\"",
                    ( *task )(); task->state = task_base::done );
            }

            is_waiting = true;
            semaphore.down();
            is_waiting = false;
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
        vector.push_back( new thread_unit( unit_name << i ) );
    }
}

void thread_pool::insert( const task_ptr& task )
{
    vector[ index ].as< thread_unit >()->insert( task );
    ++index %= vector.size();
}

OOE_NAMESPACE_END( ( ooe ) )
