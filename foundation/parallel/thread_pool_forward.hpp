/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

    #ifndef OOE_FOUNDATION_PARALLEL_THREAD_POOL_FORWARD_HPP
    #define OOE_FOUNDATION_PARALLEL_THREAD_POOL_FORWARD_HPP

#include "foundation/parallel/lock.hpp"
#include "foundation/utility/atom.hpp"
#include "foundation/utility/partial.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

template< typename >
    class task;

//--- task_value -----------------------------------------------------------------------------------
template< typename t >
    class task_value
{
protected:
    t value;

    template< typename >
        friend class result;
};

//--- task_base ------------------------------------------------------------------------------------
class task_base
{
public:
    virtual ~task_base( void ) {}

protected:
    enum state_type
    {
        wait,
        done,
        error
    };

    // can support multiple waiters by counting each waiter and incrementing semaphore for each
    atom< state_type > state;
    ooe::semaphore semaphore;

    task_base( void )
        : state( wait ), semaphore( 0 )
    {
    }

    virtual void operator ()( void ) = 0;

    friend class result_base;
    friend class thread_unit;
};

    #define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
    #define BOOST_PP_FILENAME_1 "foundation/parallel/thread_pool_forward.hpp"
    #include BOOST_PP_ITERATE()
    #undef BOOST_PP_FILENAME_1
    #undef BOOST_PP_ITERATION_LIMITS

OOE_NAMESPACE_END( ( ooe ) )

    #endif  // OOE_FOUNDATION_PARALLEL_THREAD_POOL_FORWARD_HPP

#else   // BOOST_PP_IS_ITERATING

    #define LIMIT BOOST_PP_ITERATION()

//--- task -----------------------------------------------------------------------------------------
template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
    class task< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
    : public task_base, public task_value< r >
{
public:
    typedef ooe::partial< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) > partial_type;

    task( const partial_type& partial_ )
        : task_base(), task_value< r >(), partial( partial_ )
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

    #undef LIMIT

#endif  // BOOST_PP_IS_ITERATING
