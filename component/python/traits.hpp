/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

    #ifndef OOE_COMPONENT_PYTHON_TRAITS_HPP
    #define OOE_COMPONENT_PYTHON_TRAITS_HPP

#include "component/python/traits_forward.hpp"
#include "foundation/utility/tuple.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( python ) )

//--- as -------------------------------------------------------------------------------------------
template< typename t >
    struct as< t, typename enable_if< is_sequence< t > >::type >;

template< typename t >
    struct as< t, typename enable_if< is_set< t > >::type >;

template< typename t >
    struct as< t, typename enable_if< is_map< t > >::type >;

template< typename t >
    struct as< t, typename enable_if< is_pair< t > >::type >;

//--- from -----------------------------------------------------------------------------------------
template< typename t >
    struct from< t, typename enable_if< is_sequence< t > >::type >;

template< typename t >
    struct from< t, typename enable_if< is_set< t > >::type >;

template< typename t >
    struct from< t, typename enable_if< is_map< t > >::type >;

template< typename t >
    struct from< t, typename enable_if< is_pair< t > >::type >;

//--- traits: sequence -----------------------------------------------------------------------------
template< typename t >
    struct as< t, typename enable_if< is_sequence< t > >::type >
{
    static void call( PyObject* object, typename call_traits< t >::reference container )
    {
        if ( !PyList_Check( object ) )
            throw error::python() << "Object is not a list";

        typedef typename no_ref< t >::type type;
        up_t array_size = PyList_GET_SIZE( object );

        type out;
        reserve( out, array_size );

        for ( up_t i = 0; i != array_size; ++i )
        {
            PyObject* item = PyList_GET_ITEM( object, i );

            typename type::value_type element;
            as< typename type::value_type >::call( item, element );
            out.push_back( element );
        }

        container.swap( out );
    }
};

template< typename t >
    struct from< t, typename enable_if< is_sequence< t > >::type >
{
    static PyObject* call( typename call_traits< t >::param_type container )
    {
        typedef typename no_ref< t >::type type;
        python::object object = valid( PyList_New( container.size() ) );
        up_t index = 0;

        for ( typename type::const_iterator i = container.begin(), end = container.end();
            i != end; ++i, ++index )
            PyList_SET_ITEM( object.get(), index, from< typename type::value_type >::call( *i ) );

        return object.release();
    }
};

//--- traits: set ----------------------------------------------------------------------------------
template< typename t >
    struct as< t, typename enable_if< is_set< t > >::type >
{
    static void call( PyObject* object, typename call_traits< t >::reference set )
    {
        if( !PySet_Check( object ) )
            throw error::python() << "Object is not a set";

        typedef typename no_ref< t >::type type;
        python::object iterator = valid( PyObject_GetIter( object ) );
        python::object item;

        type out;

        while ( ( item = PyIter_Next( iterator ) ) )
        {
            typename type::key_type key;
            as< typename type::key_type >::call( item, key );
            out.insert( key );
        }

        set.swap( out );
    }
};

template< typename t >
    struct from< t, typename enable_if< is_set< t > >::type >
{
    static PyObject* call( typename call_traits< t >::param_type set )
    {
        typedef typename no_ref< t >::type type;
        python::object object = PySet_New( 0 );

        for ( typename type::const_iterator i = set.begin(), end = set.end(); i != end; ++i )
        {
            python::object key = from< typename type::key_type >::call( *i );
            PySet_Add( object, key );
        }

        return object.release();
    }
};

//--- traits: map ----------------------------------------------------------------------------------
template< typename t >
    struct as< t, typename enable_if< is_map< t > >::type >
{
    static void call( PyObject* object, typename call_traits< t >::reference map )
    {
        if ( !PyDict_Check( object ) )
            throw error::python() << "Object is not a map";

        sp_t i = 0;
        PyObject* key;
        PyObject* mapped;

        typedef typename no_ref< t >::type type;
        type out;

        while ( PyDict_Next( object, &i, &key, &mapped ) )
        {
            typename type::key_type k;
            as< typename type::key_type >::call( key, k );
            typename type::mapped_type m;
            as< typename type::mapped_type >::call( mapped, m );
            out.insert( typename type::value_type( k, m ) );
        }

        map.swap( out );
    }
};

template< typename t >
    struct from< t, typename enable_if< is_map< t > >::type >
{
    static PyObject* call( typename call_traits< t >::param_type map )
    {
        typedef typename no_ref< t >::type type;
        python::object object = valid( PyDict_New() );

        for ( typename type::const_iterator i = map.begin(), end = map.end(); i != end; ++i )
        {
            python::object key = from< typename type::key_type >::call( i->first );
            python::object value = from< typename type::mapped_type >::call( i->second );

            if ( PyDict_SetItem( object, key, value ) )
                throw error::python() << "Unable to set dictionary item";
        }

        return object.release();
    }
};

//--- traits: pair ---------------------------------------------------------------------------------
template< typename t >
    struct as< t, typename enable_if< is_pair< t > >::type >
{
    static void call( PyObject* object, typename call_traits< t >::reference pair )
    {
        if ( !PyTuple_Check( object ) )
            throw error::python() << "Object is not a tuple";

        up_t tuple_size = PyTuple_GET_SIZE( object );

        if ( tuple_size != 2 )
            throw error::python() << "Tuple is of size " << tuple_size << ", pair is of size 2";

        typedef typename no_ref< t >::type type;
        as< typename type::first_type >::call( PyTuple_GET_ITEM( object, 0 ), pair.first );
        as< typename type::second_type >::call( PyTuple_GET_ITEM( object, 1 ), pair.second );
    }
};

template< typename t >
    struct from< t, typename enable_if< is_pair< t > >::type >
{
    static PyObject* call( typename call_traits< t >::param_type pair )
    {
        typedef typename no_ref< t >::type type;
        python::object object = valid( PyTuple_New( 2 ) );
        PyTuple_SET_ITEM( object.get(), 0,
            from< typename type::first_type >::call( pair.first ) );
        PyTuple_SET_ITEM( object.get(), 1,
            from< typename type::second_type >::call( pair.second ) );
        return object.release();
    }
};

OOE_NAMESPACE_END( ( ooe )( python ) )

    #define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
    #define BOOST_PP_FILENAME_1 "component/python/traits.hpp"
    #include BOOST_PP_ITERATE()
    #undef BOOST_PP_FILENAME_1
    #undef BOOST_PP_ITERATION_LIMITS

    #endif  // OOE_COMPONENT_PYTHON_TRAITS_HPP

#else   // BOOST_PP_IS_ITERATING

    #define LIMIT BOOST_PP_ITERATION()

    #define TUPLE_AS( z, n, d )\
        as< typename tuple_element< n, t >::type >::\
            call( PyTuple_GET_ITEM( object, n ), tuple._ ## n );

    #define TUPLE_FROM( z, n, d )\
        PyTuple_SET_ITEM( object.get(), n,\
            ( from< typename tuple_element< n, t >::type >::call( tuple._ ## n ) ) );

OOE_NAMESPACE_BEGIN( ( ooe )( python ) )

//--- traits: tuple --------------------------------------------------------------------------------
template< typename t >
    struct as< t, typename enable_if_c< tuple_size< t >::value == LIMIT >::type >
{
    static void call( PyObject* object, typename call_traits< t >::reference tuple )
    {
        if ( !PyTuple_Check( object ) )
            throw error::python() << "Object is not a tuple";

        up_t tuple_size = PyTuple_GET_SIZE( object );

        if ( tuple_size != LIMIT )
            throw error::python() << "Tuple is of size " << tuple_size << ", require " << LIMIT;

        BOOST_PP_REPEAT( LIMIT, TUPLE_AS, ~ )
    }
};

template< typename t >
    struct from< t, typename enable_if_c< tuple_size< t >::value == LIMIT >::type >
{
    static PyObject* call( typename call_traits< t >::param_type tuple )
    {
        python::object object = valid( PyTuple_New( LIMIT ) );
        BOOST_PP_REPEAT( LIMIT, TUPLE_FROM, ~ )
        return object.release();
    }
};

OOE_NAMESPACE_END( ( ooe )( python ) )

    #undef TUPLE_FROM
    #undef TUPLE_AS
    #undef LIMIT

#endif  // BOOST_PP_IS_ITERATING
