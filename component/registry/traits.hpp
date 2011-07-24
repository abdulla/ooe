/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_REGISTRY_TRAITS_HPP
#define OOE_COMPONENT_REGISTRY_TRAITS_HPP

#include "foundation/utility/hint.hpp"
#include "foundation/utility/tuple.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( component ) )

typedef void ( * throw_type )( void );

//--- traits: is_boolean ---------------------------------------------------------------------------
template< typename t >
    struct is_boolean
    : public is_like< t, bool >
{
};

//--- traits: is_integral --------------------------------------------------------------------------
template< typename t >
    struct is_integral
{
    static const bool value =
        !is_boolean< t >::value &&
        ooe::is_integral< typename no_ref< t >::type >::value;
};

//--- traits: is_floating_point --------------------------------------------------------------------
template< typename t >
    struct is_floating_point
    : public ooe::is_floating_point< typename no_ref< t >::type >
{
};

//--- traits: is_pointer ---------------------------------------------------------------------------
template< typename t >
    struct is_pointer
{
    static const bool value =
        !is_cstring< t >::value &&
        !is_member_pointer< typename no_ref< t >::type >::value &&
        ooe::is_pointer< typename no_ref< t >::type >::value;
};

//--- traits: is_class -----------------------------------------------------------------------------
template< typename t >
    struct is_class
{
    static const bool value =
        !is_construct< t >::value &&
        !is_destruct< t >::value &&
        !is_stdstring< t >::value &&
        !is_stdcontainer< t >::value &&
        !is_tuple< t >::value &&
        ( is_member_pointer< typename no_ref< t >::type >::value ||
        ooe::is_class< typename no_ref< t >::type >::value ||
        is_union< typename no_ref< t >::type >::value );
};

//--- meta_throw -----------------------------------------------------------------------------------
template< typename t >
    void meta_throw( void )
{
    typedef t* pointer;
    throw pointer();
}

//--- meta_catch -----------------------------------------------------------------------------------
template< typename t >
    bool meta_catch( throw_type function )
{
    if ( function == &meta_throw< t > )
        return true;

    try
    {
        function();
    }
    catch ( t* )
    {
        return true;
    }
    catch ( ... )
    {
    }

    return false;
}

OOE_NAMESPACE_END( ( ooe )( component ) )

#endif  // OOE_COMPONENT_REGISTRY_TRAITS_HPP
