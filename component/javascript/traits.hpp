/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

    #ifndef OOE_COMPONENT_JAVASCRIPT_TRAITS_HPP
    #define OOE_COMPONENT_JAVASCRIPT_TRAITS_HPP

#include "component/javascript/traits_forward.hpp"
#include "foundation/utility/tuple.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( javascript ) )

//--- to -------------------------------------------------------------------------------------------
template< typename t >
    struct to< t, typename enable_if< is_sequence< t > >::type >;

template< typename t >
    struct to< t, typename enable_if< is_set< t > >::type >;

template< typename t >
    struct to< t, typename enable_if< is_map< t > >::type >;

template< typename t >
    struct to< t, typename enable_if< is_pair< t > >::type >;

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
    struct to< t, typename enable_if< is_sequence< t > >::type >
{
    static void call( const v8::Handle< v8::Value >& value,
        typename call_traits< t >::reference container )
    {
        if ( !value->IsArray() )
            throw error::javascript() << "Value is not an array";

        v8::HandleScope scope;
        v8::Array* array = v8::Array::Cast( *value );
        up_t array_size = array->Length();

        typedef typename no_ref< t >::type type;
        type out;
        reserve( out, array_size );

        for ( up_t i = 0; i != array_size; ++i )
        {
            v8::Handle< v8::Value > item = array->Get( from< up_t >::call( i ) );

            typename type::value_type element;
            to< typename type::value_type >::call( item, element );
            out.push_back( element );
        }

        container.swap( out );
    }
};

template< typename t >
    struct from< t, typename enable_if< is_sequence< t > >::type >
{
    static v8::Handle< v8::Value > call( typename call_traits< t >::param_type container )
    {
        typedef typename no_ref< t >::type type;
        v8::Handle< v8::Array > array = v8::Array::New( container.size() );
        up_t index = 0;

        for ( typename type::const_iterator i = container.begin(), end = container.end();
            i != end; ++i, ++index )
            array->Set( from< up_t >::call( index ),
                from< typename type::value_type >::call( *i ) );

        return array;
    }
};

//--- traits: set ----------------------------------------------------------------------------------
template< typename t >
    struct to< t, typename enable_if< is_set< t > >::type >
{
    static void call( const v8::Handle< v8::Value >& value,
        typename call_traits< t >::reference set )
    {
        if ( !value->IsObject() )
            throw error::javascript() << "Value is not an object";

        v8::HandleScope scope;
        v8::Object* object = v8::Object::Cast( *value );
        v8::Handle< v8::Array > array = object->GetPropertyNames();

        typedef typename no_ref< t >::type type;
        type out;

        for ( up_t i = 0, end = array->Length(); i != end; ++i )
        {
            typename type::key_type key;
            to< typename type::key_type >::call( array->Get( from< up_t >::call( i ) ), key );
            out.insert( key );
        }

        set.swap( out );
    }
};

template< typename t >
    struct from< t, typename enable_if< is_set< t > >::type >
{
    static v8::Handle< v8::Value > call( typename call_traits< t >::param_type set )
    {
        typedef typename no_ref< t >::type type;
        v8::Handle< v8::Object > object = v8::Object::New();

        for ( typename type::const_iterator i = set.begin(), end = set.end(); i != end; ++i )
            object->Set( from< typename type::key_type >::call( *i ), v8::True() );

        return object;
    }
};

//--- traits: map ----------------------------------------------------------------------------------
template< typename t >
    struct to< t, typename enable_if< is_map< t > >::type >
{
    static void call( const v8::Handle< v8::Value >& value,
        typename call_traits< t >::reference map )
    {
        if ( !value->IsObject() )
            throw error::javascript() << "Value is not an object";

        v8::HandleScope scope;
        v8::Object* object = v8::Object::Cast( *value );
        v8::Handle< v8::Array > array = object->GetPropertyNames();

        typedef typename no_ref< t >::type type;
        type out;

        for ( up_t i = 0, end = array->Length(); i != end; ++i )
        {
            v8::Handle< v8::Value > item = array->Get( from< up_t >::call( i ) );
            v8::Handle< v8::Value > data = object->Get( item );

            typename type::key_type key;
            to< typename type::key_type >::call( item, key );
            typename type::mapped_type mapped;
            to< typename type::mapped_type >::call( data, mapped );
            out.insert( typename type::value_type( key, mapped ) );
        }

        map.swap( out );
    }
};

template< typename t >
    struct from< t, typename enable_if< is_map< t > >::type >
{
    static v8::Handle< v8::Value > call( typename call_traits< t >::param_type map )
    {
        typedef typename no_ref< t >::type type;
        v8::Handle< v8::Object > object = v8::Object::New();

        for ( typename type::const_iterator i = map.begin(), end = map.end(); i != end; ++i )
            object->Set( from< typename type::key_type >::call( i->first ),
                from< typename type::mapped_type >::call( i->second ) );

        return object;
    }
};

//--- traits: pair ---------------------------------------------------------------------------------
template< typename t >
    struct to< t, typename enable_if< is_pair< t > >::type >
{
    static void call( const v8::Handle< v8::Value >& value,
        typename call_traits< t >::reference pair )
    {
        if ( !value->IsArray() )
            throw error::javascript() << "Value is not an array";

        v8::Array* array = v8::Array::Cast( *value );
        up_t array_size = array->Length();

        if ( array_size != 2 )
            throw error::javascript() << "Array is of size " << array_size << ", pair is of size 2";

        typedef typename no_ref< t >::type type;
        v8::HandleScope scope;

        to< typename type::first_type >::
            call( array->Get( from< up_t >::call( 0 ) ), pair.first );
        to< typename type::second_type >::
            call( array->Get( from< up_t >::call( 1 ) ), pair.second );
    }
};

template< typename t >
    struct from< t, typename enable_if< is_pair< t > >::type >
{
    static v8::Handle< v8::Value > call( typename call_traits< t >::param_type pair )
    {
        typedef typename no_ref< t >::type type;
        v8::Handle< v8::Array > array = v8::Array::New( 2 );

        array->Set( from< up_t >::call( 0 ),
            from< typename type::first_type >::call( pair.first ) );
        array->Set( from< up_t >::call( 1 ),
            from< typename type::second_type >::call( pair.second ) );

        return array;
    }
};

OOE_NAMESPACE_END( ( ooe )( javascript ) )

    #define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
    #define BOOST_PP_FILENAME_1 "component/javascript/traits.hpp"
    #include BOOST_PP_ITERATE()
    #undef BOOST_PP_FILENAME_1
    #undef BOOST_PP_ITERATION_LIMITS

    #endif  // OOE_COMPONENT_JAVASCRIPT_TRAITS_HPP

#else   // BOOST_PP_IS_ITERATING

    #define LIMIT BOOST_PP_ITERATION()

    #define TUPLE_TO( z, n, d )\
        to< typename tuple_element< n, t >::type >::\
            call( array->Get( from< up_t >::call( n ) ), tuple._ ## n );

    #define TUPLE_FROM( z, n, d )\
        array->Set( from< up_t >::call( n ),\
            from< typename tuple_element< n, t >::type >::call( tuple._ ## n ) );

OOE_NAMESPACE_BEGIN( ( ooe )( javascript ) )

//--- traits: tuple --------------------------------------------------------------------------------
template< typename t >
    struct to< t, typename enable_if_c< tuple_size< t >::value == LIMIT >::type >
{
    static void call( const v8::Handle< v8::Value >& value,
        typename call_traits< t >::reference BOOST_PP_EXPR_IF( LIMIT, tuple ) )
    {
        if ( !value->IsArray() )
            throw error::javascript() << "Value is not an array";

        v8::Array* array = v8::Array::Cast( *value );
        up_t array_size = array->Length();

        if ( array_size != LIMIT )
            throw error::javascript() << "Array is of size " << array_size << ", require " << LIMIT;

        v8::HandleScope scope;
        BOOST_PP_REPEAT( LIMIT, TUPLE_TO, ~ )
    }
};

template< typename t >
    struct from< t, typename enable_if_c< tuple_size< t >::value == LIMIT >::type >
{
    static v8::Handle< v8::Value > call( typename call_traits< t >::param_type tuple )
    {
        v8::Handle< v8::Array > array = v8::Array::New( LIMIT );
        BOOST_PP_REPEAT( LIMIT, TUPLE_FROM, ~ )
        return array;
    }
};

OOE_NAMESPACE_END( ( ooe )( javascript ) )

    #undef TUPLE_FROM
    #undef TUPLE_TO
    #undef LIMIT

#endif  // BOOST_PP_IS_ITERATING
