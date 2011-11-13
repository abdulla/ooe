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
    static void call( data* data, typename call_traits< t >::reference container )
    {
        typedef typename no_ref< t >::type type;
        list list( data );
        up_t array_size = list.size();

        type out;
        reserve( out, array_size );

        for ( up_t i = 0; i != array_size; ++i )
        {
            typename type::value_type element;
            as< typename type::value_type >::call( list.get( i ), element );
            out.push_back( element );
        }

        container.swap( out );
    }
};

template< typename t >
    struct from< t, typename enable_if< is_sequence< t > >::type >
{
    static data* call( typename call_traits< t >::param_type container )
    {
        typedef typename no_ref< t >::type type;
        list list( container.size() );
        object object( list );
        up_t index = 0;

        for ( typename type::const_iterator i = container.begin(), end = container.end();
            i != end; ++i, ++index )
            list.set( index, from< typename type::value_type >::call( *i ) );

        return object.release();
    }
};

//--- traits: set ----------------------------------------------------------------------------------
template< typename t >
    struct as< t, typename enable_if< is_set< t > >::type >
{
    static void call( data* data, typename call_traits< t >::reference set )
    {
        typedef typename no_ref< t >::type type;
        python::set py_set( data );
        iterator iterator = py_set.get();
        object item;

        type out;

        while ( ( item = iterator.next() ) )
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
    static data* call( typename call_traits< t >::param_type set )
    {
        typedef typename no_ref< t >::type type;
        python::set py_set;
        object object( py_set );

        for ( typename type::const_iterator i = set.begin(), end = set.end(); i != end; ++i )
            py_set.add( from< typename type::key_type >::call( *i ) );

        return object.release();
    }
};

//--- traits: map ----------------------------------------------------------------------------------
template< typename t >
    struct as< t, typename enable_if< is_map< t > >::type >
{
    static void call( data* data, typename call_traits< t >::reference map )
    {
        typedef typename no_ref< t >::type type;
        dictionary dictionary( data );
        type out;

        for ( dictionary::tuple tuple( 0, 0, 0 ); dictionary.next( tuple ); )
        {
            typename type::key_type key;
            as< typename type::key_type >::call( tuple._1, key );
            typename type::mapped_type value;
            as< typename type::mapped_type >::call( tuple._2, value );
            out.insert( std::make_pair( key, value ) );
        }

        map.swap( out );
    }
};

template< typename t >
    struct from< t, typename enable_if< is_map< t > >::type >
{
    static data* call( typename call_traits< t >::param_type map )
    {
        typedef typename no_ref< t >::type type;
        dictionary dictionary;
        object object( dictionary );

        for ( typename type::const_iterator i = map.begin(), end = map.end(); i != end; ++i )
        {
            python::object key = from< typename type::key_type >::call( i->first );
            python::object value = from< typename type::mapped_type >::call( i->second );
            dictionary.set( key, value );
        }

        return object.release();
    }
};

//--- traits: pair ---------------------------------------------------------------------------------
template< typename t >
    struct as< t, typename enable_if< is_pair< t > >::type >
{
    static void call( data* data, typename call_traits< t >::reference pair )
    {
        tuple tuple( data );
        up_t tuple_size = tuple.size();

        if ( tuple_size != 2 )
            throw error::python() << "Tuple is of size " << tuple_size << ", pair is of size 2";

        typedef typename no_ref< t >::type type;
        as< typename type::first_type >::call( tuple.get( 0 ), pair.first );
        as< typename type::second_type >::call( tuple.get( 1 ), pair.second );
    }
};

template< typename t >
    struct from< t, typename enable_if< is_pair< t > >::type >
{
    static data* call( typename call_traits< t >::param_type pair )
    {
        typedef typename no_ref< t >::type type;
        tuple tuple( 2 );
        object object( tuple );
        tuple.set( 0, from< typename type::first_type >::call( pair.first ) );
        tuple.set( 1, from< typename type::second_type >::call( pair.second ) );
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
        as< typename tuple_element< n, t >::type >::call( py_tuple.get( n ), tuple._ ## n );

    #define TUPLE_FROM( z, n, d )\
        py_tuple.set( n, from< typename tuple_element< n, t >::type >::call( tuple._ ## n ) );

OOE_NAMESPACE_BEGIN( ( ooe )( python ) )

//--- traits: tuple --------------------------------------------------------------------------------
template< typename t >
    struct as< t, typename enable_if_c< tuple_size< t >::value == LIMIT >::type >
{
    static void call( data* data,
        typename call_traits< t >::reference BOOST_PP_EXPR_IF( LIMIT, tuple ) )
    {
        python::tuple py_tuple( data );
        up_t tuple_size = py_tuple.size();

        if ( tuple_size != LIMIT )
            throw error::python() <<
                "Tuple is of size " << tuple_size << ", expected a size of " << LIMIT;

        BOOST_PP_REPEAT( LIMIT, TUPLE_AS, ~ )
    }
};

template< typename t >
    struct from< t, typename enable_if_c< tuple_size< t >::value == LIMIT >::type >
{
    static data* call( typename call_traits< t >::param_type BOOST_PP_EXPR_IF( LIMIT, tuple ) )
    {
        python::tuple py_tuple( LIMIT );
        object object( py_tuple );
        BOOST_PP_REPEAT( LIMIT, TUPLE_FROM, ~ )
        return object.release();
    }
};

OOE_NAMESPACE_END( ( ooe )( python ) )

    #undef TUPLE_FROM
    #undef TUPLE_AS
    #undef LIMIT

#endif  // BOOST_PP_IS_ITERATING
