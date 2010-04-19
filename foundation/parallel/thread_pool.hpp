/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_FOUNDATION_PARALLEL_THREAD_POOL_HPP
	#define OOE_FOUNDATION_PARALLEL_THREAD_POOL_HPP

#include <vector>

#include "foundation/parallel/thread_pool_forward.hpp"
#include "foundation/utility/atom.hpp"
#include "foundation/utility/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

typedef atom_ptr< task_base > task_type;

//--- result_base ----------------------------------------------------------------------------------
class result_base
{
protected:
	task_type task;

	result_base( const task_type& task_ )
		: task( task_ )
	{
	}

	void wait( void )
	{
		lock lock( task->mutex );

		while ( task->state == task_base::wait )
			task->condition.wait( lock );

		if ( task->state == task_base::error )
			throw error::runtime( "result: " ) << "An error occured processing the result";
	}
};

//--- result ---------------------------------------------------------------------------------------
template< typename t >
	struct result
	: private result_base
{
	result( const task_type& task_ )
		: result_base( task_ )
	{
	}

	t& operator ()( void )
	{
		wait();
		return dynamic_cast< task_value< t >& >( *task ).value;
	}
};

template<>
	struct result< void >
	: private result_base
{
	result( const task_type& task_ )
		: result_base( task_ )
	{
	}

	void operator ()( void )
	{
		wait();
	}
};

//--- thread_pool ----------------------------------------------------------------------------------
class thread_pool
{
public:
	thread_pool( void ) OOE_VISIBLE;
	void insert( const task_type& ) OOE_VISIBLE;

private:
	typedef std::vector< opaque_ptr > vector_type;

	up_t index;
	vector_type vector;
};

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "foundation/parallel/thread_pool.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

OOE_NAMESPACE_END( ( ooe ) )

	#endif	// OOE_FOUNDATION_PARALLEL_THREAD_POOL_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()

template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
	result< r > async( thread_pool& pool,
	const function< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >& function
	BOOST_PP_ENUM_TRAILING_BINARY_PARAMS( LIMIT, typename call_traits< t, >::param_type a ) )
{
	typedef task< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) > t;
	typedef typename t::partial_type p;
	task_type task = new t( p( function BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) ) );
	pool.insert( task );
	return task;
}

	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
