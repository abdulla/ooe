/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/executable/environment.hpp"
#include "foundation/parallel/scheduler.hpp"
#include "foundation/utility/atom.hpp"
#include "foundation/utility/error.hpp"

namespace ooe
{
	typedef worker_list_type::iterator worker_iterator;

//--- worker -------------------------------------------------------------------
	class worker
	{
	public:
		worker_iterator iterator;
		atom< bool > state;

		worker( const thread::function_type& function )
			: iterator(), state( true ), thread( function, this )
		{
		}

	private:
		ooe::thread thread;
	};

//--- scheduler ----------------------------------------------------------------
	scheduler::scheduler( up_t workers )
		: worker_mutex(), worker_condition(), worker_list(), task_mutex(), task_condition(),
		task_list()
	{
		resize( workers ? workers : executable::cpu_cores() * 2 - 1 );
	}

	scheduler::~scheduler( void )
	{
		resize( 0 );
		lock lock( worker_mutex );

		for ( up_t workers = worker_list.size(); workers--; )
			push_back( function< void ( void ) >() );

		while ( !worker_list.empty() )
			worker_condition.wait( lock );
	}

	up_t scheduler::size( void ) const
	{
		lock lock( worker_mutex );
		return worker_list.size();
	}

	void scheduler::resize( up_t workers )
	{
		lock lock( worker_mutex );
		worker_iterator i = worker_list.begin();
		worker_iterator end = worker_list.end();
		up_t j = 0;

		while ( i != end && j < workers )
			++i, ++j;

		if ( j == workers )
		{
			// worker will quit after next execution
			for ( ; i != end; ++i )
				( *i )->state = false;
		}
		else
		{
			for ( ; j != workers; ++j )
			{
				worker_list.push_back( new worker( make_function( *this, &scheduler::call ) ) );
				worker_list.back()->iterator = --worker_list.end();
			}
		}
	}

	void scheduler::push_back( task_type task )
	{
		lock lock( task_mutex );
		task_list.push_back( task );
		task_condition.notify_one();
	}

	task_type scheduler::pop_front( void )
	{
		lock lock( task_mutex );

		while ( task_list.empty() )
			task_condition.wait( lock );

		task_type task = task_list.front();
		task_list.pop_front();
		return task;
	}

	void* scheduler::call( void* pointer )
	{
		ooe::worker& worker = *static_cast< ooe::worker* >( pointer );

		while ( worker.state )
		{
			task_type task = pop_front();
			OOE_PRINT( "scheduler", task->call() );
			task->condition.notify_all();
		}

		{
			lock lock( worker_mutex );
			worker_list.erase( worker.iterator );
		}

		worker_condition.notify_one();
		return 0;
	}
}
