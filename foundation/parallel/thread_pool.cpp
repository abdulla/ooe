/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>
#include <list>

#include "foundation/executable/environment.hpp"
#include "foundation/parallel/queue.hpp"
#include "foundation/parallel/thread.hpp"
#include "foundation/parallel/thread_pool.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- thread_unit ----------------------------------------------------------------------------------
class thread_unit
{
public:
    thread_unit( const std::string& name )
        : mutex(), condition(), queue(), state( true ),
        thread( name, make_function( *this, &thread_unit::main ), 0 )
    {
    }

    ~thread_unit( void )
    {
        state = false;
        condition.notify_one();
        thread.join();
    }

    void insert( const task_ptr& task )
    {
        queue.enqueue( task );
        condition.notify_one();
    }

private:
    ooe::mutex mutex;
    ooe::condition condition;
    ooe::queue< task_ptr > queue;

    atom< bool > state;
    ooe::thread thread;

    void* main( void* )
    {
        while ( state )
        {
            lock lock( mutex );

            for ( task_ptr task; queue.dequeue( task ); task->condition.notify_all() )
            {
                task->state = task_base::error;
                OOE_PRINT( "thread_pool \"" << thread.name() << "\"",
                    ( *task )(); task->state = task_base::done );
            }

            condition.wait( lock );
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
        vector.push_back( opaque_ptr( new thread_unit( unit_name << i ), destroy< thread_unit > ) );
    }
}

void thread_pool::insert( const task_ptr& task )
{
    vector[ index ].as< thread_unit >()->insert( task );
    ++index %= vector.size();
}

OOE_NAMESPACE_END( ( ooe ) )
