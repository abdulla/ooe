/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>
#include <list>

#include "foundation/executable/environment.hpp"
#include "foundation/parallel/thread.hpp"
#include "foundation/parallel/thread_pool.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- thread_unit ----------------------------------------------------------------------------------
class thread_unit
{
public:
    thread_unit( const std::string& name )
        : state( true ), thread( name, make_function( *this, &thread_unit::main ), 0 )
    {
    }

    ~thread_unit( void )
    {
        state = false;
        insert( new task< void ( void ) >( function< void ( void ) >() ) );
        thread.join();
    }

    void insert( const task_ptr& task )
    {
        {
            lock lock( mutex );
            list.push_back( task );
        }
        condition.notify_one();
    }

private:
    ooe::mutex mutex;
    ooe::condition condition;
    std::list< task_ptr > list;

    atom< bool > state;
    ooe::thread thread;

    task_ptr pop_front( void )
    {
        lock lock( mutex );

        while ( list.empty() )
            condition.wait( lock );

        task_ptr task = list.front();
        list.pop_front();
        return task;
    }

    void* main( void* )
    {
        while ( state )
        {
            task_ptr task = pop_front();
            {
                lock lock( task->mutex );
                task->state = task_base::error;
                OOE_PRINT( "thread_pool \"" << thread.name() << "\"",
                    ( *task )(); task->state = task_base::done );
            }
            task->condition.notify_all();
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
        std::string unit_name = name + '-';
        vector.push_back( opaque_ptr( new thread_unit( unit_name << i ), destroy< thread_unit > ) );
    }
}

void thread_pool::insert( const task_ptr& task )
{
    vector[ index ].as< thread_unit >()->insert( task );
    ++index %= vector.size();
}

OOE_NAMESPACE_END( ( ooe ) )
