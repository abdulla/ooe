/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "component/javascript/facade.hpp"
#include "component/registry/local.hpp"
#include "component/registry/registry.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe )( javascript ) )

typedef std::vector< std::string > find_vector;
typedef shared_ptr< ooe::source > source_ptr;

template< v8::InvocationCallback function >
    struct embed
{
    struct defer
    {
        static v8::Handle< v8::Value > call( const v8::Arguments& arguments )
        {
            return function( arguments );
        }
    };

    static v8::Handle< v8::Value > call( const v8::Arguments& arguments )
    {
        return invoke< defer >( arguments );
    }
};

v8::Handle< v8::Value > find( const v8::Arguments& arguments )
{
    verify_arguments( arguments, 1 );

    find_vector input;
    to< find_vector >::call( arguments[ 0 ], input );
    interface interface;

    for ( find_vector::const_iterator i = input.begin(), end = input.end(); i != end; ++i )
    {
        up_t j = i->find( '/' );

        if ( j == ~up_t( 0 ) )
            throw error::runtime( "javascript::find: " ) << "Invalid function \"" << *i << '\"';

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

v8::Handle< v8::Value > load( const v8::Arguments& arguments )
{
    verify_arguments( arguments, 1 );

    std::string path;
    to< std::string >::call( arguments[ 0 ], path );

    source_ptr source( new ooe::source( path ) );
    const module& module = source->get();
    const interface::vector_type& names = module.names();
    const facade::local::vector_type& local = static_cast< const facade::local* >
        ( module.find( typeid( facade::local ).name() ) )->get();
    const facade::javascript::vector_type& javascript = static_cast< const facade::javascript* >
        ( module.find( typeid( facade::javascript ).name() ) )->get();

    v8::Handle< v8::Object > object = v8::Object::New();

    for ( up_t i = 0, end = names.size(); i != end; ++i )
    {
        v8::Handle< v8::Value > key =
            from< std::string >::call( names[ i ]._0 + '/' + names[ i ]._1 );
        v8::Handle< v8::Value > data = javascript[ i ]._0( local[ i ] );
        object->Set( key, v8::FunctionTemplate::New( javascript[ i ]._1, data )->GetFunction() );
    }

    object->Set( from< const c8* >::call( "" ), from< source_ptr >::call( source ) );
    return object;
}

v8::Handle< v8::Value > doc( const v8::Arguments& arguments )
{
    verify_arguments( arguments, 2 );

    if ( !arguments[ 0 ]->IsObject() )
        throw error::javascript() << "Value is not an object";

    v8::Object* object = v8::Object::Cast( *arguments[ 0 ] );

    source_ptr source;
    std::string value;
    to< source_ptr >::call( object->Get( from< const c8* >::call( "" ) ), source );
    to< std::string >::call( arguments[ 1 ], value );

    up_t i = value.find( '/' );

    if ( i == ~up_t( 0 ) )
        throw error::runtime( "javascript::doc: " ) << "Function \"" << value << "\" has no type";

    const c8* documentation = source->get().doc( value.substr( 0, i ), value.substr( i + 1 ) );
    return from< const c8* >::call( documentation );
}

v8::Handle< v8::Value > print( const v8::Arguments& arguments )
{
    for ( up_t i = 0, end = arguments.Length(); i != end; ++i )
    {
        if ( i )
            std::cout << '\t';

        v8::String::Utf8Value utf8( arguments[ i ] );
        std::cout << *utf8;
    }

    std::cout << '\n';
    return v8::Undefined();
}

OOE_ANONYMOUS_END( ( ooe )( javascript ) )

OOE_NAMESPACE_BEGIN( ( ooe )( facade ) )

//--- javascript -----------------------------------------------------------------------------------
const javascript::vector_type& javascript::get( void ) const
{
    return vector;
}

void javascript::insert( up_t index, ooe::javascript::from_type from, v8::InvocationCallback call )
{
    vector_type::iterator i = vector.begin();
    std::advance( i, index );
    vector.insert( i, make_tuple( from, call ) );
}

OOE_NAMESPACE_END( ( ooe )( facade ) )

OOE_NAMESPACE_BEGIN( ( ooe )( javascript ) )

//--- component_setup ------------------------------------------------------------------------------
void component_setup( v8::Handle< v8::Object > global )
{
    v8::Handle< v8::Object > ooe = v8::Object::New();

    //--- registry ---------------------------------------------------------
    v8::Handle< v8::Object > registry = v8::Object::New();

    registry->Set( from< const c8* >::call( "find" ),
        v8::FunctionTemplate::New( embed< find >::call )->GetFunction() );
    registry->Set( from< const c8* >::call( "load" ),
        v8::FunctionTemplate::New( embed< load >::call )->GetFunction() );
    registry->Set( from< const c8* >::call( "doc" ),
        v8::FunctionTemplate::New( embed< doc >::call )->GetFunction() );

    ooe->Set( from< const c8* >::call( "component" ), registry );

    //----------------------------------------------------------------------

    global->Set( from< const c8* >::call( "print" ),
        v8::FunctionTemplate::New( embed< print >::call )->GetFunction() );

    global->Set( from< const c8* >::call( "ooe" ), ooe );
}

//--- throw_exception ------------------------------------------------------------------------------
void throw_exception( const c8* what, const c8* where )
{
    std::string string;
    string << what << "\n\nStack trace:" << where << "\n\nSource line:";

    v8::Handle< v8::String > error = v8::String::New( string.data(), string.size() );
    v8::ThrowException( v8::Exception::Error( error ) );
}

OOE_NAMESPACE_END( ( ooe )( javascript ) )
