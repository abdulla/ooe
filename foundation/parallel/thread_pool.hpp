/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_FOUNDATION_PARALLEL_THREAD_POOL_HPP
	#define OOE_FOUNDATION_PARALLEL_THREAD_POOL_HPP

#include <vector>

#include "foundation/parallel/lock.hpp"
#include "foundation/parallel/thread.hpp"
#include "foundation/utility/atom.hpp"
#include "foundation/utility/partial.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

template< typename >
	class task;

template< typename >
	class future;

//--- result ---------------------------------------------------------------------------------------
template< typename t >
	class result
{
protected:
	t value;

	friend class future< t >;
};

//--- task_base ------------------------------------------------------------------------------------
class task_base
{
public:
	virtual ~task_base( void ) {}

protected:
	ooe::mutex mutex;
	ooe::condition condition;

	task_base( void );
	virtual void operator ()( void ) = 0;

	friend class thread_unit;
};

typedef atom_ptr< task_base > task_type;

	#define TASK
	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "foundation/parallel/thread_pool.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS
	#undef TASK

//--- future ---------------------------------------------------------------------------------------
template< typename t >
	class future
{
public:
	future( const task_type& task_ )
		: task( task_ )
	{
	}

private:
	task_type task;
};

//--- thread_pool ----------------------------------------------------------------------------------
class thread_pool
{
public:
	typedef std::vector< opaque_ptr > vector_type;

	thread_pool( void );
	~thread_pool( void );

	#define INSERT
	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "foundation/parallel/thread_pool.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS
	#undef INSERT

private:
	vector_type vector;
};

OOE_NAMESPACE_END( ( ooe ) )

	#endif	// OOE_FOUNDATION_PARALLEL_THREAD_POOL_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()

	#ifdef TASK
//--- task -----------------------------------------------------------------------------------------
template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
	class task< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	: public task_base, public result< r >
{
public:
	typedef ooe::partial< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) > partial_type;

	task( const partial_type& partial_ )
		: task_base(), result< r >(), partial( partial_ )
	{
	}

private:
	partial_type partial;

	virtual void operator ()( void )
	{
		this->value = partial();
	}
};

template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
	class task< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	: public task_base
{
public:
	typedef ooe::partial< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) > partial_type;

	task( const partial_type& partial_ )
		: task_base(), partial( partial_ )
	{
	}

private:
	partial_type partial;

	virtual void operator ()( void )
	{
		partial();
	}
};
	#endif

	#ifdef INSERT
	template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		future< r >
		insert( BOOST_PP_ENUM_BINARY_PARAMS( LIMIT, typename call_traits< t, >::param_type a ) )
	{
	}
	#endif

	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
