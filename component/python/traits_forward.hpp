/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_PYTHON_TRAITS_FORWARD_HPP
#define OOE_COMPONENT_PYTHON_TRAITS_FORWARD_HPP

#include "component/python/error.hpp"
#include "component/python/object.hpp"
#include "foundation/utility/miscellany.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( python ) )

//--- get_pointer ----------------------------------------------------------------------------------
template< typename t >
    t* get_pointer( const capsule& capsule )
{
    if ( component::meta_catch< t >( capsule.function() ) )
        return byte_cast< t* >( capsule.pointer() );

    throw error::python() << "Bad argument, \"" << demangle( typeid( t ).name() ) << "\" expected";
}

//--- destroy --------------------------------------------------------------------------------------
template< typename t >
    void destroy( data* data_ )
{
    delete static_cast< t* >( capsule( data_ ).pointer() );
}

//--- traits: default ------------------------------------------------------------------------------
template< typename NO_SPECIALISATION_DEFINED, typename = void >
    struct as
{
    static void call( data*, NO_SPECIALISATION_DEFINED ) OOE_CONST
    {
        OOE_STATIC_ASSERT( !sizeof( NO_SPECIALISATION_DEFINED ) );
    }
};

template< typename NO_SPECIALISATION_DEFINED, typename = void >
    struct from
{
    static data* call( NO_SPECIALISATION_DEFINED ) OOE_CONST
    {
        OOE_STATIC_ASSERT( !sizeof( NO_SPECIALISATION_DEFINED ) );
        return 0;
    }
};

//--- traits: empty --------------------------------------------------------------------------------
template< typename t >
    struct as< t, typename enable_if< is_empty< t > >::type >
{
    static void call( data* data_, typename call_traits< t >::reference )
    {
        if ( !is_none( data_ ) )
            throw error::python() << "Object is not none";
    }
};

template< typename t >
    struct from< t, typename enable_if< is_empty< t > >::type >
{
    static data* call( typename call_traits< t >::param_type )
    {
        return none();
    }
};

//--- traits: boolean ------------------------------------------------------------------------------
template< typename t >
    struct as< t, typename enable_if< component::is_boolean< t > >::type >
{
    static void call( data* data_, bool& boolean )
    {
        boolean = python::boolean( data_ ).get();
    }
};

template< typename t >
    struct from< t, typename enable_if< component::is_boolean< t > >::type >
{
    static data* call( bool boolean )
    {
        return python::boolean( boolean );
    }
};

//--- traits: integral -----------------------------------------------------------------------------
template< typename t >
    struct as< t, typename enable_if< component::is_integral< t > >::type >
{
    static void call( data* data_, typename call_traits< t >::reference integral )
    {
        integral = python::integral( data_ ).get();
    }
};

template< typename t >
    struct from< t, typename enable_if< component::is_integral< t > >::type >
{
    static data* call( typename call_traits< t >::param_type integral )
    {
        return python::integral( integral );
    }
};

//--- traits: floating-point -----------------------------------------------------------------------
template< typename t >
    struct as< t, typename enable_if< component::is_floating_point< t > >::type >
{
    static void call( data* data_, typename call_traits< t >::reference floating_point )
    {
        floating_point = python::floating_point( data_ ).get();
    }
};

template< typename t >
    struct from< t, typename enable_if< component::is_floating_point< t > >::type >
{
    static data* call( typename call_traits< t >::param_type floating_point )
    {
        return python::floating_point( floating_point );
    }
};

//--- traits: string -------------------------------------------------------------------------------
template< typename t >
    struct as< t, typename enable_if< is_string< t > >::type >
{
    static void call( data* data_, typename call_traits< t >::reference string )
    {
        string::tuple tuple = python::string( data_ ).get();
        string = string_make< typename no_ref< t >::type >( tuple._0, tuple._1 );
    }
};

template< typename t >
    struct from< t, typename enable_if< is_string< t > >::type >
{
    static data* call( typename call_traits< t >::param_type string )
    {
        return python::string( string_data( string ), string_size( string ) );
    }
};

//--- traits: pointer ------------------------------------------------------------------------------
template< typename t >
    struct as< t, typename enable_if< component::is_pointer< t > >::type >
{
    static void call( data* data_, typename call_traits< t >::reference pointer )
    {
        typedef typename no_ref< t >::type type;
        pointer = get_pointer< typename no_qual< t >::type >( capsule( data_ ) );
    }
};

template< typename t >
    struct from< t, typename enable_if< component::is_pointer< t > >::type >
{
    static data* call( typename call_traits< t >::param_type pointer )
    {
        typedef typename no_qual< t >::type type;
        return capsule( byte_cast< void* >( pointer ), component::meta_throw< type > );
    }
};

//--- traits: class --------------------------------------------------------------------------------
template< typename t >
    struct as< t, typename enable_if< component::is_class< t > >::type >
{
    static void call( data* data_, typename call_traits< t >::reference class_ )
    {
        typedef typename no_ref< t >::type type;
        typedef typename remove_member_const< type >::type meta_type;
        class_ = *get_pointer< meta_type >( capsule( data_ ) );
    }
};

template< typename t >
    struct from< t, typename enable_if< component::is_class< t > >::type >
{
    static data* call( typename call_traits< t >::param_type class_ )
    {
        typedef typename no_ref< t >::type type;
        typedef typename remove_member_const< type >::type meta_type;
        return capsule( new type( class_ ), component::meta_throw< meta_type >, destroy< type > );
    }
};

//--- traits: construct ----------------------------------------------------------------------------
template< typename INVALID_USAGE >
    struct as< INVALID_USAGE, typename enable_if< is_construct< INVALID_USAGE > >::type >
{
    static void call( data*, INVALID_USAGE )
    {
        OOE_STATIC_ASSERT( !sizeof( INVALID_USAGE ) );
    }
};

template< typename t >
    struct from< t, typename enable_if< is_construct< t > >::type >
{
    static data* call( typename call_traits< t >::param_type construct )
    {
        typedef typename t::value_type type;
        return capsule( construct, component::meta_throw< type >, destroy< type > );
    }
};

//--- traits: destruct -----------------------------------------------------------------------------
template< typename t >
    struct as< t, typename enable_if< is_destruct< t > >::type >
{
    static void call( data* data_, typename call_traits< t >::reference destruct )
    {
        typedef typename t::value_type type;
        capsule capsule( data_ );
        destruct = get_pointer< type >( capsule );
        capsule.reset();
    }
};

template< typename INVALID_USAGE >
    struct from< INVALID_USAGE, typename enable_if< is_destruct< INVALID_USAGE > >::type >
{
    static data* call( INVALID_USAGE )
    {
        OOE_STATIC_ASSERT( !sizeof( INVALID_USAGE ) );
        return 0;
    }
};

//--- traits: array --------------------------------------------------------------------------------
template< typename t >
    struct as< t, typename enable_if< is_array< t > >::type >
{
    static void call( data* data_, typename call_traits< t >::reference array )
    {
        typedef typename no_ref< t >::type type;
        list list( data_ );
        up_t py_size = list.size();
        up_t array_size = extent< type >::value;

        if ( py_size != array_size )
            throw error::python() << "Array in python is of size " << py_size <<
                ", array is of size " << array_size;

        for ( up_t i = 0; i != array_size; ++i )
            as< typename remove_extent< type >::type >::call( list.get( i ), array[ i ] );
    }
};

template< typename t >
    struct from< t, typename enable_if< is_array< t > >::type >
{
    static data* call( typename call_traits< t >::param_type array )
    {
        typedef typename no_ref< t >::type type;
        up_t array_size = extent< type >::value;
        list list( array_size );
        object object( list );

        for ( up_t i = 0; i != array_size; ++i )
            list.set( i, from< typename remove_extent< type >::type >::call( array[ i ] ) );

        return object.release();
    }
};

OOE_NAMESPACE_END( ( ooe )( python ) )

#endif  // OOE_COMPONENT_PYTHON_TRAITS_FORWARD_HPP
