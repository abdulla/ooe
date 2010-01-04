/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_FOUNDATION_PARALLEL_TASK_HPP
	#define OOE_FOUNDATION_PARALLEL_TASK_HPP

#include "foundation/parallel/lock.hpp"
#include "foundation/parallel/thread.hpp"
#include "foundation/utility/partial.hpp"

namespace ooe
{
	template< typename >
		class unique_task;

	template< typename >
		class task;

//--- task_base ----------------------------------------------------------------
	struct task_base
	{
		bool done;
		mutable ooe::mutex mutex;
		mutable ooe::condition condition;

		task_base( void )
			: done( false ), mutex(), condition()
		{
		}

		virtual ~task_base( void ) {}
		virtual void call( void ) = 0;
	};

//--- task_value ---------------------------------------------------------------
	template< typename type >
		struct task_value
	{
		type value;
	};
}

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "foundation/parallel/task.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	#endif	// OOE_FOUNDATION_PARALLEL_TASK_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()

namespace ooe
{
//--- unique_task --------------------------------------------------------------
	template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		class unique_task< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
	public:
		typedef ooe::partial< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) > partial_type;
		typedef typename partial_type::function_type function_type;

		unique_task( const function_type& function
			BOOST_PP_ENUM_TRAILING_BINARY_PARAMS( LIMIT, t, a ) )
			: partial( function BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) ), value(),
			thread( make_function( *this, &unique_task::call ), 0 )
		{
		}

		~unique_task( void )
		{
			thread.join();
		}

		r operator ()( void )
		{
			thread.join();
			return value;
		}

	private:
		partial_type partial;
		r value;
		ooe::thread thread;

		void* call( void* )
		{
			value = partial();
			return 0;
		}
	};

	template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
		class unique_task< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
	public:
		typedef ooe::partial< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) > partial_type;
		typedef BOOST_PP_EXPR_IF( LIMIT, typename ) partial_type::function_type function_type;

		unique_task( const function_type& function
			BOOST_PP_ENUM_TRAILING_BINARY_PARAMS( LIMIT, t, a ) )
			: partial( function BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) ),
			thread( make_function( *this, &unique_task::call ), 0 )
		{
		}

		void operator ()( void )
		{
			thread.join();
		}

	private:
		partial_type partial;
		ooe::thread thread;

		void* call( void* )
		{
			partial();
			return 0;
		}
	};

//--- task ---------------------------------------------------------------------
	template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		class task< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
		: public task_base, private task_value< r >
	{
	public:
		typedef ooe::partial< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) > partial_type;
		typedef typename partial_type::function_type function_type;

		task( const function_type& function BOOST_PP_ENUM_TRAILING_BINARY_PARAMS( LIMIT, t, a ) )
			: task_base(), task_value< r >(),
			partial( function BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) )
		{
		}

	private:
		partial_type partial;

		virtual void call( void )
		{
			lock lock( mutex );
			this->value = partial();
			done = true;
		}
	};

	template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
		class task< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
		: public task_base
	{
	public:
		typedef ooe::partial< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) > partial_type;
		typedef BOOST_PP_EXPR_IF( LIMIT, typename ) partial_type::function_type function_type;

		task( const function_type& function BOOST_PP_ENUM_TRAILING_BINARY_PARAMS( LIMIT, t, a ) )
			: task_base(), partial( function BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) )
		{
		}

	private:
		partial_type partial;

		virtual void call( void )
		{
			lock lock( mutex );
			partial();
			done = true;
		}
	};
}

	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
