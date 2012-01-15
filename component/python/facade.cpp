/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "component/python/facade.hpp"
#include "component/registry/local.hpp"
#include "component/registry/registry.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe )( python ) )

typedef std::vector< std::string > find_vector;
typedef shared_ptr< ooe::source > source_ptr;

template< cfunction function >
    struct embed
{
    struct defer
    {
        static data* call( data* self, data* arguments )
        {
            return function( self, arguments );
        }
    };

    static data* call( data* self, data* arguments )
    {
        return invoke< defer >( self, arguments );
    }
};

data* find( data*, data* list )
{
    find_vector input;
    as< find_vector >::call( list, input );
    interface interface;

    for ( find_vector::const_iterator i = input.begin(), end = input.end(); i != end; ++i )
    {
        up_t j = i->find( '/' );

        if ( j == ~up_t( 0 ) )
            throw error::runtime( "python::find: " ) << "Invalid function \"" << *i << '\"';

        interface.insert( i->substr( 0, j ), i->substr( j + 1 ) );
    }

    registry registry;
    registry::info_vector v = registry.find( interface );
    find_vector output;

    for ( registry::info_vector::const_iterator i = v.begin(), end = v.end(); i != end; ++i )
    {
        if ( i->_0 == registry::library )
            output.push_back( i->_1 );
    }

    return from< find_vector >::call( output );
}

data* load( data*, data* string )
{
    std::string path;
    as< std::string >::call( string, path );

    source_ptr source( new ooe::source( path ) );
    const ooe::module& module = source->get();
    const interface::vector_type& names = module.names();
    const ooe::module::vector_type& docs = module.docs();
    const facade::local::vector_type& local = static_cast< const facade::local* >
        ( module.find( typeid( facade::local ).name() ) )->get();
    const facade::python::vector_type& python = static_cast< const facade::python* >
        ( module.find( typeid( facade::python ).name() ) )->get();

    dictionary dictionary;
    object object( dictionary );

    for ( up_t i = 0, end = names.size(); i != end; ++i )
    {
        python::method& method = const_cast< python::method& >( python[ i ]._1 );
        method.set( names[ i ]._0.c_str(), docs[ i ] );

        std::string name = names[ i ]._0 + '/' + names[ i ]._1;
        python::string key( name.c_str(), name.size() );
        python::object self = python[ i ]._0( local[ i ] );
        python::function value( method, self );
        dictionary.set( key, value );
    }

    dictionary.set( python::string( 0, 0 ), from< source_ptr >::call( source ) );
    return object.release();
}

data* doc( data*, data* arguments )
{
    python::tuple tuple = verify_arguments( arguments, 2 );
    dictionary dictionary( tuple.get( 0 ) );

    source_ptr source;
    std::string value;
    as< source_ptr >::call( dictionary.get( string( 0, 0 ) ), source );
    as< std::string >::call( tuple.get( 1 ), value );

    up_t i = value.find( '/' );

    if ( i == ~up_t( 0 ) )
        throw error::runtime( "python::doc: " ) << "Function \"" << value << "\" has no type";

    const c8* documentation = source->get().doc( value.substr( 0, i ), value.substr( i + 1 ) );
    return from< const c8* >::call( documentation );
}

method find_method( "find", embed< find >::call, method::single );
method load_method( "load", embed< load >::call, method::single );
method doc_method( "doc", embed< doc >::call, method::multiple );

OOE_ANONYMOUS_END( ( ooe )( python ) )

OOE_NAMESPACE_BEGIN( ( ooe )( facade ) )

//--- python ---------------------------------------------------------------------------------------
const python::vector_type& python::get( void ) const
{
    return vector;
}

void python::insert( up_t index, ooe::python::from_type from, ooe::python::cfunction call )
{
    vector_type::iterator i = vector.begin();
    std::advance( i, index );

    ooe::python::method method( 0, call, ooe::python::method::multiple );
    vector.insert( i, make_tuple( from, method ) );
}

OOE_NAMESPACE_END( ( ooe )( facade ) )

OOE_NAMESPACE_BEGIN( ( ooe )( python ) )

//--- component_setup ------------------------------------------------------------------------------
void component_setup( data* globals )
{
    module ooe( "ooe", module::make );
    ooe.add( "__file__", none() );

    //--- registry -------------------------------------------------------------
    module registry( "component", module::make );
    registry.add( "__file__", none() );

    registry.add( "find", function( find_method, 0 ) );
    registry.add( "load", function( load_method, 0 ) );
    registry.add( "doc", function( doc_method, 0 ) );

    ooe.add( "component", registry );

    //--------------------------------------------------------------------------

    dictionary( globals ).set( string( "ooe", 3 ), ooe );
}

//--- verify_arguments -----------------------------------------------------------------------------
tuple verify_arguments( data* arguments, s32 size )
{
    tuple tuple( arguments );
    s32 argument_size = tuple.size();

    if ( argument_size < size )
        throw error::python() << "Not enough arguments to function, " << size <<
            " expected, got " << argument_size;

    return tuple;
}

OOE_NAMESPACE_END( ( ooe )( python ) )
