/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

    #ifndef OOE_FOUNDATION_UTILITY_FUNCTION_HPP
    #define OOE_FOUNDATION_UTILITY_FUNCTION_HPP

#include "foundation/utility/macro.hpp"
#include "foundation/utility/miscellany.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

template< typename >
    struct invoke_function;

template< typename, typename, typename >
    struct invoke_member;

template< typename, typename, typename >
    struct invoke_pointer;

template< typename >
    struct invoke_null;

template< typename >
    struct function;

//--- make_function --------------------------------------------------------------------------------
template< typename type, typename pointer >
    function< typename remove_callable< pointer >::type > make_function( type& t, pointer p )
{
    return function< typename remove_callable< pointer >::type >( t, p );
}

template< typename pointer >
    function< typename remove_callable< pointer >::type > make_function( pointer p )
{
    return function< typename remove_callable< pointer >::type >( p );
}

OOE_NAMESPACE_END( ( ooe ) )

    #define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
    #define BOOST_PP_FILENAME_1 "foundation/utility/function.hpp"
    #include BOOST_PP_ITERATE()
    #undef BOOST_PP_FILENAME_1
    #undef BOOST_PP_ITERATION_LIMITS

    #endif  // OOE_FOUNDATION_UTILITY_FUNCTION_HPP

#else   // BOOST_PP_IS_ITERATING

    #define LIMIT BOOST_PP_ITERATION()

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- invoke_function ------------------------------------------------------------------------------
template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
    struct invoke_function< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
{
    static r call( const bound_any& any BOOST_PP_ENUM_TRAILING_BINARY_PARAMS
        ( LIMIT, typename call_traits< t, >::param_type a ) )
    {
        return reinterpret_cast< r ( * )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >( any.function )
            ( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
    }
};

//--- invoke_member --------------------------------------------------------------------------------
template< typename s, typename t, typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
    struct invoke_member< s, t, r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
{
    static r call( const bound_any& any BOOST_PP_ENUM_TRAILING_BINARY_PARAMS
        ( LIMIT, typename call_traits< t, >::param_type a ) )
    {
        return ( static_cast< s* >( any.object.pointer )->*
            reinterpret_cast< r ( t::* )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
            ( any.object.member ) )( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
    }
};

//--- invoke_pointer -------------------------------------------------------------------------------
template< typename s, typename t, typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
    struct invoke_pointer< s, t, r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
{
    static r call( const bound_any& any BOOST_PP_ENUM_TRAILING_BINARY_PARAMS
        ( LIMIT, typename call_traits< t, >::param_type a ) )
    {
        return reinterpret_cast< r ( * )( t& BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, t ) ) >
            ( any.object.function )( *static_cast< s* >( any.object.pointer )
            BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );
    }
};

//--- invoke_null ----------------------------------------------------------------------------------
template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
    struct invoke_null< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
{
    static r call( const bound_any& BOOST_PP_ENUM_TRAILING_BINARY_PARAMS
        ( LIMIT, typename call_traits< t, >::param_type BOOST_PP_INTERCEPT ) ) OOE_CONST
    {
        return r();
    }
};

//--- function -------------------------------------------------------------------------------------
template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
    class function< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
{
public:
    function( void )
        : call( invoke_null< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >::call ), pointer( 0 )
    {
    }

    function( r ( * function_ )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) )
        : call( invoke_function< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >::call ),
        pointer( function_ )
    {
    }

    template< typename s, typename t >
        function( s& object, r ( t::* member )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) )
        : call( invoke_member< s, t, r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >::call ),
        pointer( bound_any( &object, member ) )
    {
    }

    template< typename s, typename t >
        function( s& object, r ( t::* member )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) const )
        : call( invoke_member< s, t, r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >::call ),
        pointer( bound_any( &object,
            reinterpret_cast< r ( t::* )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >( member ) ) )
    {
    }

    template< typename s, typename t >
        function( const s& object, r ( t::* member )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) const )
        : call( invoke_member< s, t, r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >::call ),
        pointer( bound_any( &object,
            reinterpret_cast< r ( t::* )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >( member ) ) )
    {
    }

    template< typename s, typename t >
        function( s& object, r ( * function_ )( t& BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, t ) ) )
        : call( invoke_pointer< s, t, r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >::call ),
        pointer( bound_any( &object, function_ ) )
    {
    }

    template< typename s, typename t >
        function( const s& object,
        r ( * function_ )( const t& BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, t ) ) )
        : call( invoke_pointer< s, t, r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >::call ),
        pointer( bound_any( &object, function_ ) )
    {
    }

    void clear( void )
    {
        this->call = invoke_null< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >::call;
    }

    r operator ()( BOOST_PP_ENUM_BINARY_PARAMS
        ( LIMIT, typename call_traits< t, >::param_type a ) ) const
    {
        return call( pointer BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );
    }

private:
    typedef r ( * call_type )( const bound_any& BOOST_PP_ENUM_TRAILING_BINARY_PARAMS
        ( LIMIT, typename call_traits< t, >::param_type BOOST_PP_INTERCEPT ) );

    call_type call;
    bound_any pointer;
};

OOE_NAMESPACE_END( ( ooe ) )

    #undef LIMIT

#endif  // BOOST_PP_IS_ITERATING
