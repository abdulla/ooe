/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

    #ifndef OOE_FOUNDATION_UTILITY_PARTIAL_HPP
    #define OOE_FOUNDATION_UTILITY_PARTIAL_HPP

#include "foundation/utility/function.hpp"
#include "foundation/utility/tuple.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

template< typename >
    class partial;

OOE_NAMESPACE_END( ( ooe ) )

    #define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
    #define BOOST_PP_FILENAME_1 "foundation/utility/partial.hpp"
    #include BOOST_PP_ITERATE()
    #undef BOOST_PP_FILENAME_1
    #undef BOOST_PP_ITERATION_LIMITS

    #endif  // OOE_FOUNDATION_UTILITY_PARTIAL_HPP

#else   // BOOST_PP_IS_ITERATING

    #define LIMIT BOOST_PP_ITERATION()

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- partial --------------------------------------------------------------------------------------
template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
    class partial< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
{
public:
    typedef ooe::function< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) > function_type;

    partial( function_type function_ BOOST_PP_ENUM_TRAILING_BINARY_PARAMS
        ( LIMIT, typename call_traits< t, >::param_type a ) )
        : function( function_ ) BOOST_PP_COMMA_IF( LIMIT )
        BOOST_PP_EXPR_IF( LIMIT, tuple( BOOST_PP_ENUM_PARAMS( LIMIT, a ) ) )
    {
    }

    r operator ()( void ) const
    {
        return function( BOOST_PP_ENUM_PARAMS( LIMIT, tuple._ ) );
    }

    void clear( void )
    {
        function.clear();
    }

private:
    typedef ooe::tuple< BOOST_PP_ENUM_PARAMS( LIMIT, t ) > tuple_type;

    function_type function;
    BOOST_PP_EXPR_IF( LIMIT, tuple_type tuple; )
};

OOE_NAMESPACE_END( ( ooe ) )

    #undef LIMIT

#endif  // BOOST_PP_IS_ITERATING
