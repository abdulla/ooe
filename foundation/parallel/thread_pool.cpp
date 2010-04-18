/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>
#include <list>

#include "foundation/executable/environment.hpp"
#include "foundation/parallel/thread_pool.hpp"
#include "foundation/utility/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- thread_unit ----------------------------------------------------------------------------------
class thread_unit
{
public:
	thread_unit( void )
		: state( true ), thread( thread::function_type( *this, &thread_unit::call ), 0 )
	{
	}

	~thread_unit( void )
	{
		state = false;
		// push_back empty task
		thread.join();
	}

	void push_back( const task_type& task )
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
	std::list< task_type > list;

	atom< bool > state;
	ooe::thread thread;

	task_type pop_front( void )
	{
		lock lock( mutex );

		while ( list.empty() )
			condition.wait( lock );

		task_type task = list.front();
		list.pop_front();
		return task;
	}

	void* call( void* )
	{
		while ( state )
		{
			task_type task = pop_front();
			OOE_PRINT( "thread_pool", ( *task )() );
			task->condition.notify_all();
		}

		return 0;
	}
};

//--- thread_pool ----------------------------------------------------------------------------------
thread_pool::thread_pool( void )
	: vector()
{
	for ( up_t i = 0, end = executable::cpu_cores(); i != end; ++i )
		vector.push_back( opaque_ptr( new thread_unit, destroy< thread_unit > ) );
}

OOE_NAMESPACE_END( ( ooe ) )
