/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

    #ifndef OOE_FOUNDATION_UTILITY_HINT_HPP
    #define OOE_FOUNDATION_UTILITY_HINT_HPP

#include "foundation/utility/traits.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- proxy_ptr ------------------------------------------------------------------------------------
template< typename t >
    class proxy_ptr
{
public:
    typedef t value_type;
    typedef t* pointer;
    typedef t& reference;

    operator pointer( void ) const
    {
        return value;
    }

    pointer operator ->( void ) const
    {
        return value;
    }

    reference operator *( void ) const
    {
        return *value;
    }

protected:
    proxy_ptr( pointer value_ )
        : value( value_ )
    {
    }

private:
    pointer value;
};

//--- construct_ptr --------------------------------------------------------------------------------
template< typename t >
    struct construct_ptr
    : public proxy_ptr< t >
{
    construct_ptr( t* value_ = 0 )
        : proxy_ptr< t >( value_ )
    {
    }
};

//--- destruct_ptr ---------------------------------------------------------------------------------
template< typename t >
    struct destruct_ptr
    : public proxy_ptr< t >
{
    destruct_ptr( t* value_ = 0 )
        : proxy_ptr< t >( value_ )
    {
    }
};

//--- is_construct ---------------------------------------------------------------------------------
template< typename t >
    struct is_construct
    : public is_template1< t, construct_ptr >
{
};

//--- is_destruct ----------------------------------------------------------------------------------
template< typename t >
    struct is_destruct
    : public is_template1< t, destruct_ptr >
{
};

//--- destruct -------------------------------------------------------------------------------------
template< typename t >
    void destruct( destruct_ptr< t > value )
{
    delete value;
}

    #define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
    #define BOOST_PP_FILENAME_1 "foundation/utility/hint.hpp"
    #include BOOST_PP_ITERATE()
    #undef BOOST_PP_FILENAME_1
    #undef BOOST_PP_ITERATION_LIMITS

OOE_NAMESPACE_END( ( ooe ) )

    #endif  // OOE_FOUNDATION_UTILITY_HINT_HPP

#else   // BOOST_PP_IS_ITERATING

    #define LIMIT BOOST_PP_ITERATION()

//--- construct ------------------------------------------------------------------------------------
template< typename t BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
    construct_ptr< t > construct( BOOST_PP_ENUM_BINARY_PARAMS( LIMIT, t, a ) )
{
    return new BOOST_PP_IF( LIMIT, t( BOOST_PP_ENUM_PARAMS( LIMIT, a ) ), t );
}

    #undef LIMIT

#endif  // BOOST_PP_IS_ITERATING
