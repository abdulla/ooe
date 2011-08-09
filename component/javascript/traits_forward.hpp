/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_JAVASCRIPT_TRAITS_FORWARD_HPP
#define OOE_COMPONENT_JAVASCRIPT_TRAITS_FORWARD_HPP

#include "component/javascript/error.hpp"
#include "component/javascript/vm.hpp"
#include "foundation/utility/miscellany.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( javascript ) )

//--- destroy --------------------------------------------------------------------------------------
template< typename t >
    void destroy( v8::Persistent< v8::Value > value, void* )
{
    v8::Object* object = v8::Object::Cast( *value );

    if ( object->InternalFieldCount() != 2 )
        throw error::javascript() << "Object has no internal fields";

    delete ptr_cast< t* >( object->GetPointerFromInternalField( 0 ) );
    value.Dispose();
}

//--- traits: default ------------------------------------------------------------------------------
template< typename NO_SPECIALISATION_DEFINED, typename = void >
    struct to
{
    static void call( const v8::Handle< v8::Value >&, NO_SPECIALISATION_DEFINED ) OOE_CONST
    {
        OOE_STATIC_ASSERT( !sizeof( NO_SPECIALISATION_DEFINED ) );
    }
};

template< typename NO_SPECIALISATION_DEFINED, typename = void >
    struct from
{
    static v8::Handle< v8::Value > call( NO_SPECIALISATION_DEFINED ) OOE_CONST
    {
        OOE_STATIC_ASSERT( !sizeof( NO_SPECIALISATION_DEFINED ) );
        return v8::Undefined();
    }
};

//--- traits: empty --------------------------------------------------------------------------------
template< typename t >
    struct to< t, typename enable_if< is_empty< t > >::type >
{
    static void call( const v8::Handle< v8::Value >& value, typename call_traits< t >::reference )
    {
        if ( !value->IsNull() )
            throw error::javascript() << "Value is not null";
    }
};

template< typename t >
    struct from< t, typename enable_if< is_empty< t > >::type >
{
    static v8::Handle< v8::Value > call( typename call_traits< t >::param_type )
    {
        return v8::Null();
    }
};

//--- traits: boolean ------------------------------------------------------------------------------
template< typename t >
    struct to< t, typename enable_if< component::is_boolean< t > >::type >
{
    static void call( const v8::Handle< v8::Value >& value, bool& boolean )
    {
        if ( !value->IsBoolean() )
            throw error::javascript() << "Value is not a boolean";

        boolean = value->ToBoolean()->Value();
    }
};

template< typename t >
    struct from< t, typename enable_if< component::is_boolean< t > >::type >
{
    static v8::Handle< v8::Value > call( bool boolean )
    {
        return v8::Boolean::New( boolean );
    }
};

//--- traits: integral -----------------------------------------------------------------------------
template< typename t >
    struct to< t, typename enable_if< component::is_integral< t > >::type >
{
    static void call( const v8::Handle< v8::Value >& value,
        typename call_traits< t >::reference integral )
    {
        if ( !value->IsInt32() )
            throw error::javascript() << "Value is not an integral number";

        integral = value->ToInteger()->Value();
    }
};

template< typename t >
    struct from< t, typename enable_if< component::is_integral< t > >::type >
{
    static v8::Handle< v8::Value > call( typename call_traits< t >::param_type integral )
    {
        return v8::Integer::New( integral );
    }
};

//--- traits: floating-point -----------------------------------------------------------------------
template< typename t >
    struct to< t, typename enable_if< component::is_floating_point< t > >::type >
{
    static void call( const v8::Handle< v8::Value >& value,
        typename call_traits< t >::reference floating_point )
    {
        if ( !value->IsNumber() )
            throw error::javascript() << "Value is not a floating-point number";

        floating_point = value->ToNumber()->Value();
    }
};

template< typename t >
    struct from< t, typename enable_if< component::is_floating_point< t > >::type >
{
    static v8::Handle< v8::Value > call( typename call_traits< t >::param_type floating_point )
    {
        return v8::Number::New( floating_point );
    }
};

//--- traits: string -------------------------------------------------------------------------------
template< typename t >
    struct to< t, typename enable_if< is_string< t > >::type >
{
    static void call( const v8::Handle< v8::Value >& value,
        typename call_traits< t >::reference string )
    {
        if ( !value->IsString() )
            throw error::javascript() << "Value is not a string";

        v8::String::Utf8Value utf8( value );
        string = string_make< typename no_ref< t >::type >( *utf8, utf8.length() );
    }
};

template< typename t >
    struct from< t, typename enable_if< is_string< t > >::type >
{
    static v8::Handle< v8::Value > call( typename call_traits< t >::param_type string )
    {
        return v8::String::New( string_data( string ), string_size( string ) );
    }
};

//--- traits: pointer ------------------------------------------------------------------------------
template< typename t >
    struct to< t, typename enable_if< component::is_pointer< t > >::type >
{
    static void call( const v8::Handle< v8::Value >& value,
        typename call_traits< t >::reference pointer )
    {
        if ( !value->IsObject() )
            throw error::javascript() << "Value is not an object";

        v8::Object* object = v8::Object::Cast( *value );

        if ( object->InternalFieldCount() != 2 )
            throw error::javascript() << "Object does not have required internal fields";

        typedef typename no_qual< t >::type type;
        component::throw_type function =
            ptr_cast< component::throw_type >( object->GetPointerFromInternalField( 1 ) );

        if ( !component::meta_catch< type >( function ) )
            throw error::javascript() <<
                "Bad argument \"" << demangle( typeid( type ).name() ) << "\" expected";

        pointer =
            ptr_cast< typename no_ref< t >::type >( object->GetPointerFromInternalField( 0 ) );
    }
};

template< typename t >
    struct from< t, typename enable_if< component::is_pointer< t > >::type >
{
    static v8::Handle< v8::Value > call( typename call_traits< t >::param_type pointer )
    {
        component::throw_type function = component::meta_throw< typename no_qual< t >::type >;
        return make_object( ptr_cast( pointer ), function );
    }
};

//--- traits: class --------------------------------------------------------------------------------
template< typename t >
    struct to< t, typename enable_if< component::is_class< t > >::type >
{
    static void call( const v8::Handle< v8::Value >& value,
        typename call_traits< t >::reference class_ )
    {
        typedef typename no_ref< t >::type type;
        typedef typename remove_member_const< type >::type meta_type;
        meta_type* pointer;
        to< meta_type* >::call( value, pointer );
        class_ = *( type* )pointer;
    }
};

template< typename t >
    struct from< t, typename enable_if< component::is_class< t > >::type >
{
    static v8::Handle< v8::Value > call( typename call_traits< t >::param_type class_ )
    {
        typedef typename no_ref< t >::type type;
        typedef typename remove_member_const< type >::type meta_type;
        return from< construct_ptr< meta_type > >::call( ( meta_type* )new type( class_ ) );
    }
};

//--- traits: construct ----------------------------------------------------------------------------
template< typename INVALID_USAGE >
    struct to< INVALID_USAGE, typename enable_if< is_construct< INVALID_USAGE > >::type >
{
    static void call( const v8::Handle< v8::Value >&, INVALID_USAGE ) OOE_CONST
    {
        OOE_STATIC_ASSERT( !sizeof( INVALID_USAGE ) );
    }
};

template< typename t >
    struct from< t, typename enable_if< is_construct< t > >::type >
{
    static v8::Handle< v8::Value > call( typename call_traits< t >::param_type construct )
    {
        typedef typename t::value_type type;
        component::throw_type function = component::meta_throw< type >;
        v8::Persistent< v8::Object > object =
            v8::Persistent< v8::Object >::New( make_object( construct, function ) );
        object.MakeWeak( 0, destroy< type > );
        return object;
    }
};

//--- traits: destruct -----------------------------------------------------------------------------
template< typename t >
    struct to< t, typename enable_if< is_destruct< t > >::type >
{
    static void call( const v8::Handle< v8::Value >& value,
        typename call_traits< t >::reference destruct )
    {
        typedef typename t::pointer pointer;
        pointer p;
        to< pointer >::call( value, p );
        destruct = p;

        v8::Object::Cast( *value )->SetPointerInInternalField( 0, 0 );
        v8::Object::Cast( *value )->SetPointerInInternalField( 1, 0 );
    }
};

template< typename INVALID_USAGE >
    struct from< INVALID_USAGE, typename enable_if< is_destruct< INVALID_USAGE > >::type >
{
    static v8::Handle< v8::Value > call( INVALID_USAGE )
    {
        OOE_STATIC_ASSERT( !sizeof( INVALID_USAGE ) );
        return v8::Undefined();
    }
};

//--- traits: array --------------------------------------------------------------------------------
template< typename t >
    struct to< t, typename enable_if< is_array< t > >::type >
{
    static void call( const v8::Handle< v8::Value >& value,
        typename call_traits< t >::reference array )
    {
        if ( !value->IsArray() )
            throw error::javascript() << "Value is not an array";

        typedef typename no_ref< t >::type type;
        v8::Array* js_array = v8::Array::Cast( *value );
        up_t js_size = js_array->Length();
        up_t array_size = extent< type >::value;

        if ( js_size != array_size )
            throw error::javascript() << "Array in javascript is of size " << js_size <<
                ", array is of size " << array_size;

        v8::HandleScope scope;

        for ( up_t i = 0; i != array_size; ++i )
            to< typename remove_extent< type >::type >::
                call( js_array->Get( from< up_t >::call( i ) ), array[ i ] );
    }
};

template< typename t >
    struct from< t, typename enable_if< is_array< t > >::type >
{
    static v8::Handle< v8::Value > call( typename call_traits< t >::param_type array )
    {
        typedef typename no_ref< t >::type type;
        up_t array_size = extent< type >::value;
        v8::Handle< v8::Array > handle = v8::Array::New( array_size );

        for ( up_t i = 0; i != array_size; ++i )
            handle->Set( from< up_t >::call( i ),
                from< typename remove_extent< type >::type >::call( array[ i ] ) );

        return handle;
    }
};

OOE_NAMESPACE_END( ( ooe )( javascript ) )

#endif  // OOE_COMPONENT_JAVASCRIPT_TRAITS_FORWARD_HPP
