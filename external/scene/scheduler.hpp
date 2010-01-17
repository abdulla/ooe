/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_EXTERNAL_SCENE_SCHEDULER_HPP
	#define OOE_EXTERNAL_SCENE_SCHEDULER_HPP

#include <list>

#include "external/scene/task.hpp"
#include "foundation/utility/atom.hpp"

namespace ooe
{
	class worker;

	typedef shared_ptr< worker > worker_ptr;
	typedef std::list< worker_ptr > worker_list_type;
	typedef atom_ptr< task_base > task_type;

//--- result -------------------------------------------------------------------
	template< typename type >
		class result
	{
	public:
		result( task_type task_ )
			: task( task_ )
		{
		}

		type& operator ()( void ) const
		{
			lock lock( task->mutex );

			while ( !task->done )
				task->condition.wait( lock );

			return static_cast< task_value< type >& >( task ).value;
		}

	private:
		task_type task;
	};

	template<>
		class result< void >
	{
	public:
		result( task_type task_ )
			: task( task_ )
		{
		}

		void operator ()( void ) const
		{
			lock lock( task->mutex );

			while ( !task->done )
				task->condition.wait( lock );
		}

	private:
		task_type task;
	};

//--- scheduler ----------------------------------------------------------------
	class OOE_VISIBLE scheduler
		: private noncopyable
	{
	public:
		scheduler( up_t = 0 );
		~scheduler( void );

		up_t size( void ) const;
		void resize( up_t );

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "external/scene/scheduler.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	private:
		typedef std::list< task_type > task_list_type;

		mutable ooe::mutex worker_mutex;
		ooe::condition worker_condition;
		worker_list_type worker_list;

		ooe::mutex task_mutex;
		ooe::condition task_condition;
		task_list_type task_list;

		void push_back( task_type );
		task_type pop_front( void ) OOE_HIDDEN;
		void* call( void* ) OOE_HIDDEN;
	};
}

	#endif	// OOE_EXTERNAL_SCENE_SCHEDULER_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()

		template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
			result< r > push_back( const ooe::function< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >&
			function BOOST_PP_ENUM_TRAILING_BINARY_PARAMS( LIMIT, t, a ) )
		{
			task_type task = new ooe::task< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
				( function BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );
			push_back( task );
			return task;
		}

	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
