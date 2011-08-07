/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

    #ifndef OOE_COMPONENT_LUA_FACADE_HPP
    #define OOE_COMPONENT_LUA_FACADE_HPP

#include <vector>

#include "component/lua/traits.hpp"
#include "foundation/utility/miscellany.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( lua ) )

typedef void ( * push_type )( stack&, any );

template< typename >
    struct invoke_function;

template< typename, typename >
    struct invoke_member;

void component_setup( stack ) OOE_VISIBLE;
void throw_exception( state*, const c8*, const c8* ) OOE_VISIBLE;

//--- verify_arguments -----------------------------------------------------------------------------
inline stack verify_arguments( state* state_, u32 size )
{
    stack stack( state_ );
    u32 stack_size = stack.size();

    if ( stack_size < size )
        throw error::lua() <<
            "Not enough arguments to function, " << size << " expected, got " << stack_size;

    return stack;
}

//--- invoke ---------------------------------------------------------------------------------------
template< typename type >
    s32 invoke( state* state_ )
{
    try
    {
        return type::call( state_ );
    }
    catch ( error::runtime& error )
    {
        throw_exception( state_, error.what(), error.where() );
    }
    catch ( std::exception& error )
    {
        throw_exception( state_, error.what(), "\nNo stack trace available" );
    }
    catch ( ... )
    {
        throw_exception( state_, "An unknown exception was thrown",
            "\nNo stack trace available" );
    }

    return 0;
}

//--- push_invoke ----------------------------------------------------------------------------------
template< typename type, typename object, object ( any::* member ) >
    void push_invoke( stack& stack, any any )
{
    push< type >::call( stack, reinterpret_cast< type >( any.*member ) );
}

OOE_NAMESPACE_END( ( ooe )( lua ) )

OOE_NAMESPACE_BEGIN( ( ooe )( facade ) )

//--- lua ------------------------------------------------------------------------------------------
class lua
{
public:
    typedef tuple< ooe::lua::push_type, ooe::lua::cfunction > tuple_type;
    typedef std::vector< tuple_type > vector_type;

    const vector_type& get( void ) const OOE_VISIBLE;
    void insert( up_t, ooe::lua::push_type, ooe::lua::cfunction ) OOE_VISIBLE;

    template< typename type >
        void insert( up_t index,
        typename enable_if< is_function_pointer< type > >::type* = 0 )
    {
        typedef typename remove_pointer< type >::type function_type;
        insert( index, ooe::lua::push_invoke< type, any::function_type, &any::function >,
            ooe::lua::invoke< ooe::lua::invoke_function< function_type > > );
    }

    template< typename type >
        void insert( up_t index,
        typename enable_if< is_member_function_pointer< type > >::type* = 0 )
    {
        typedef typename member_of< type >::type object_type;
        typedef typename remove_member< type >::type member_type;
        insert( index, ooe::lua::push_invoke< type, any::member_type, &any::member >,
            ooe::lua::invoke< ooe::lua::invoke_member< object_type, member_type > > );
    }

private:
    vector_type vector;
};

OOE_NAMESPACE_END( ( ooe )( facade ) )

    #define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
    #define BOOST_PP_FILENAME_1 "component/lua/facade.hpp"
    #include BOOST_PP_ITERATE()
    #undef BOOST_PP_FILENAME_1
    #undef BOOST_PP_ITERATION_LIMITS

    #endif  // OOE_COMPONENT_LUA_FACADE_HPP

#else   // BOOST_PP_IS_ITERATING

    #define LIMIT BOOST_PP_ITERATION()

    #define TO( z, n, _ )\
        typename no_ref< t ## n >::type a ## n;\
        to< typename no_ref< t ## n >::type >::call( stack, a ## n, n + 1 );

OOE_NAMESPACE_BEGIN( ( ooe )( lua ) )

//--- invoke_function ------------------------------------------------------------------------------
template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
    struct invoke_function< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
{
    static s32 call( state* state_ )
    {
        stack stack = verify_arguments( state_, LIMIT );

        void ( * function )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
        to< void ( * )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >::call( stack, function, upvalue( 1 ) );

        BOOST_PP_REPEAT( LIMIT, TO, ~ )
        function( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );

        return 0;
    }
};

template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
    struct invoke_function< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
{
    static s32 call( state* state_ )
    {
        stack stack = verify_arguments( state_, LIMIT );

        r ( * function )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
        to< r ( * )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >::call( stack, function, upvalue( 1 ) );

        BOOST_PP_REPEAT( LIMIT, TO, ~ )
        r value = function( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
        push< r >::call( stack, value );

        return 1;
    }
};

#if LIMIT
//--- invoke_member --------------------------------------------------------------------------------
template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
    struct invoke_member< t0, void ( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) ) >
{
    static s32 call( state* state_ )
    {
        stack stack = verify_arguments( state_, LIMIT );

        void ( t0::* member )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) );
        to< void ( t0::* )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) ) >::
            call( stack, member, upvalue( 1 ) );

        t0* a0;
        to< t0* >::call( stack, a0, 1 );
        BOOST_PP_REPEAT_FROM_TO( 1, LIMIT, TO, ~ )
        ( a0->*member )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, a ) );

        return 0;
    }
};

template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
    struct invoke_member< t0, r ( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) ) >
{
    static s32 call( state* state_ )
    {
        stack stack = verify_arguments( state_, LIMIT );

        r ( t0::* member )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) );
        to< r ( t0::* )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) ) >::
            call( stack, member, upvalue( 1 ) );

        t0* a0;
        to< t0* >::call( stack, a0, 1 );
        BOOST_PP_REPEAT_FROM_TO( 1, LIMIT, TO, ~ )
        r value = ( a0->*member )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, a ) );
        push< r >::call( stack, value );

        return 1;
    }
};
#endif

OOE_NAMESPACE_END( ( ooe )( lua ) )

    #undef TO
    #undef LIMIT

#endif  // BOOST_PP_IS_ITERATING
