/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

    #ifndef OOE_COMPONENT_PYTHON_FACADE_HPP
    #define OOE_COMPONENT_PYTHON_FACADE_HPP

#include <vector>

#include "component/python/traits.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( python ) )

typedef object ( * from_type )( any );

template< typename >
    struct invoke_function;

template< typename, typename >
    struct invoke_member;

void component_setup( PyObject* ) OOE_VISIBLE;
void throw_exception( const c8*, const c8* ) OOE_VISIBLE;

//--- verify_arguments -----------------------------------------------------------------------------
inline void verify_arguments( PyObject* arguments, s32 size )
{
    s32 argument_size = PyTuple_GET_SIZE( arguments );

    if ( argument_size < size )
        throw error::python() << "Not enough arguments to function, " << size <<
            " expected, got " << argument_size;
}

//--- invoke ---------------------------------------------------------------------------------------
template< typename type >
    PyObject* invoke( PyObject* self, PyObject* arguments )
{
    try
    {
        return type::call( self, arguments );
    }
    catch ( error::runtime& error )
    {
        throw_exception( error.what(), error.where() );
    }
    catch ( std::exception& error )
    {
        throw_exception( error.what(), "\nNo stack trace available" );
    }
    catch ( ... )
    {
        throw_exception( "An unknown exception was thrown", "\nNo stack trace available" );
    }

    return 0;
}

//--- from_invoke ----------------------------------------------------------------------------------
template< typename type, typename object, object ( any::* member ) >
    python::object from_invoke( any any )
{
    return from< type >::call( reinterpret_cast< type >( any.*member ) );
}

OOE_NAMESPACE_END( ( ooe )( python ) )

OOE_NAMESPACE_BEGIN( ( ooe )( facade ) )

//--- python ---------------------------------------------------------------------------------------
class python
{
public:
    typedef tuple< ooe::python::from_type, PyMethodDef > tuple_type;
    typedef std::vector< tuple_type > vector_type;

    const vector_type& get( void ) const OOE_VISIBLE;
    void insert( up_t, ooe::python::from_type, PyCFunction ) OOE_VISIBLE;

    template< typename type >
        void insert( up_t index,
        typename enable_if< is_function_pointer< type > >::type* = 0 )
    {
        typedef typename remove_pointer< type >::type function_type;
        insert( index, ooe::python::from_invoke< type, any::function_type, &any::function >,
            ooe::python::invoke< ooe::python::invoke_function< function_type > > );
    }

    template< typename type >
        void insert( up_t index,
        typename enable_if< is_member_function_pointer< type > >::type* = 0 )
    {
        typedef typename member_of< type >::type object_type;
        typedef typename remove_member< type >::type member_type;
        insert( index, ooe::python::from_invoke< type, any::member_type, &any::member >,
            ooe::python::invoke< ooe::python::invoke_member< object_type, member_type > > );
    }

private:
    vector_type vector;
};

OOE_NAMESPACE_END( ( ooe )( facade ) )

    #define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
    #define BOOST_PP_FILENAME_1 "component/python/facade.hpp"
    #include BOOST_PP_ITERATE()
    #undef BOOST_PP_FILENAME_1
    #undef BOOST_PP_ITERATION_LIMITS

    #endif  // OOE_COMPONENT_PYTHON_FACADE_HPP

#else   // BOOST_PP_IS_ITERATING

    #define LIMIT BOOST_PP_ITERATION()

    #define AS( z, n, _ )\
        typename no_ref< t ## n >::type a ## n;\
        as< typename no_ref< t ## n >::type >::call( PyTuple_GET_ITEM( arguments, n ), a ## n );

OOE_NAMESPACE_BEGIN( ( ooe )( python ) )

//--- invoke_function ------------------------------------------------------------------------------
template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
    struct invoke_function< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
{
    static PyObject* call( PyObject* self, PyObject* arguments )
    {
        verify_arguments( arguments, LIMIT );

        void ( * function )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
        as< void ( * )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >::call( self, function );

        BOOST_PP_REPEAT( LIMIT, AS, ~ )
        function( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
        Py_RETURN_NONE;
    }
};

template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
    struct invoke_function< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
{
    static PyObject* call( PyObject* self, PyObject* arguments )
    {
        verify_arguments( arguments, LIMIT );

        r ( * function )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
        as< r ( * )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >::call( self, function );

        BOOST_PP_REPEAT( LIMIT, AS, ~ )
        r value = function( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
        return from< r >::call( value );
    }
};

#if LIMIT
//--- invoke_member --------------------------------------------------------------------------------
template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
    struct invoke_member< t0, void ( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) ) >
{
    static PyObject* call( PyObject* self, PyObject* arguments )
    {
        verify_arguments( arguments, LIMIT );

        void ( t0::* member )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) );
        as< void ( t0::* )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) ) >::call( self, member );

        t0* a0;
        as< t0* >::call( PyTuple_GET_ITEM( arguments, 0 ), a0 );
        BOOST_PP_REPEAT_FROM_TO( 1, LIMIT, AS, ~ )
        ( a0->*member )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, a ) );
        Py_RETURN_NONE;
    }
};

template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
    struct invoke_member< t0, r ( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) ) >
{
    static PyObject* call( PyObject* self, PyObject* arguments )
    {
        verify_arguments( arguments, LIMIT );

        r ( t0::* member )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) );
        as< r ( t0::* )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) ) >::call( self, member );

        t0* a0;
        as< t0* >::call( PyTuple_GET_ITEM( arguments, 0 ), a0 );
        BOOST_PP_REPEAT_FROM_TO( 1, LIMIT, AS, ~ )
        r value = ( a0->*member )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, a ) );
        return from< r >::call( value );
    }
};
#endif

OOE_NAMESPACE_END( ( ooe )( python ) )

    #undef AS
    #undef LIMIT

#endif  // BOOST_PP_IS_ITERATING
