/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

    #ifndef OOE_FOUNDATION_UTILITY_MISCELLANY_HPP
    #define OOE_FOUNDATION_UTILITY_MISCELLANY_HPP

#include "foundation/utility/traits.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- add ------------------------------------------------------------------------------------------
template< typename type >
    type* add( void* pointer, sp_t bytes )
{
    return reinterpret_cast< type* >( static_cast< u8* >( pointer ) + bytes );
}

template< typename type >
    const type* add( const void* pointer, sp_t bytes )
{
    return reinterpret_cast< const type* >( static_cast< const u8* >( pointer ) + bytes );
}

//--- skip_out -------------------------------------------------------------------------------------
template< typename in_t, typename out_t, typename skip_t >
    out_t skip_out( in_t i, in_t end, out_t j, skip_t x )
{
    for ( ; i != end; ++i, j += x )
        *j = *i;

    return j;
}

//--- skip_in --------------------------------------------------------------------------------------
template< typename in_t, typename out_t, typename skip_t >
    out_t skip_in( in_t i, in_t end, out_t j, skip_t x )
{
    for ( ; i != end; i += x, ++j )
        *j = *i;

    return j;
}

//--- ptr_cast -------------------------------------------------------------------------------------
template< typename type >
    type ptr_cast( void* pointer )
{
    union { void* in; type out; } pun;
    pun.in = pointer;
    return pun.out;
}

template< typename type >
    void* ptr_cast( type pointer )
{
    union { type in; void* out; } pun;
    pun.in = pointer;
    return pun.out;
}

//--- destroy --------------------------------------------------------------------------------------
template< typename type >
    void destroy( const void* object )
{
    delete static_cast< const type* >( object );
}

//--- destruct -------------------------------------------------------------------------------------
template< typename type >
    void destruct( type& object )
{
    object.~type();
}

template< typename type >
    void destruct( type* object )
{
    object->~type();
}

//--- any ------------------------------------------------------------------------------------------
union any
{
    typedef void ( * function_type )( void );
    typedef void ( any::* member_type )( void );

    void* pointer;
    function_type function;
    member_type member;

    any( void* pointer_ )
        : pointer( pointer_ )
    {
    }

    template< typename type >
        any( type function_, typename enable_if< is_function_pointer< type > >::type* = 0 )
        : function( reinterpret_cast< function_type >( function_ ) )
    {
    }

    template< typename type >
        any( type member_, typename enable_if< is_member_function_pointer< type > >::type* = 0 )
        : member( reinterpret_cast< member_type >( member_ ) )
    {
    }
};

//--- bound_any ------------------------------------------------------------------------------------
union bound_any
{
    typedef void ( * function_type )( void );
    typedef void ( bound_any::* member_type )( void );

    function_type function;

    struct
    {
        void* pointer;

        union
        {
            member_type member;
            function_type function;
        };
    } object;

    template< typename type >
        bound_any( type function_ )
        : function( reinterpret_cast< function_type >( function_ ) )
    {
    }

    template< typename type >
        bound_any( const void* pointer, type member,
        typename enable_if< is_member_function_pointer< type > >::type* = 0 )
    {
        object.pointer = const_cast< void* >( pointer );
        object.member = reinterpret_cast< member_type >( member );
    }

    template< typename type >
        bound_any( const void* pointer, type function_,
        typename enable_if< is_function_pointer< type > >::type* = 0 )
    {
        object.pointer = const_cast< void* >( pointer );
        object.function = reinterpret_cast< function_type >( function_ );
    }
};

//--- dual_function --------------------------------------------------------------------------------
template< typename t >
    union dual_function
{
    void ( * in )( void );
    t out;
};

//--- dual_member ----------------------------------------------------------------------------------
template< typename t >
    union dual_member
{
    void ( any::* in )( void );
    t out;
};

    #define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
    #define BOOST_PP_FILENAME_1 "foundation/utility/miscellany.hpp"
    #include BOOST_PP_ITERATE()
    #undef BOOST_PP_FILENAME_1
    #undef BOOST_PP_ITERATION_LIMITS

OOE_NAMESPACE_END( ( ooe ) )

    #endif  // OOE_FOUNDATION_UTILITY_MISCELLANY_HPP

#else   // BOOST_PP_IS_ITERATING

    #define LIMIT BOOST_PP_ITERATION()

//--- create ---------------------------------------------------------------------------------------
template< typename type BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
    void* create( BOOST_PP_ENUM_BINARY_PARAMS( LIMIT, t, a ) )
{
    return new BOOST_PP_IF( LIMIT, type( BOOST_PP_ENUM_PARAMS( LIMIT, a ) ), type );
}

//--- overload -------------------------------------------------------------------------------------
template< typename r, typename type BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
    r ( type::* overload( r ( type::* member )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) ) )
    ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) )
{
    return member;
}

template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
    r ( * overload( r ( * function )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) ) )
    ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) )
{
    return function;
}

    #undef LIMIT

#endif  // BOOST_PP_IS_ITERATING
