/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_JAVASCRIPT_TRAITS_FORWARD_HPP
#define OOE_COMPONENT_JAVASCRIPT_TRAITS_FORWARD_HPP

#include <v8.h>

#include "component/javascript/error.hpp"
#include "component/registry/traits.hpp"
#include "foundation/utility/miscellany.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( javascript ) )

//--- to -------------------------------------------------------------------------------------------
template< typename, typename = void >
	struct to;

template< typename t >
	struct to< t, typename enable_if< is_empty< t > >::type >;

template< typename t >
	struct to< t, typename enable_if< component::is_boolean< t > >::type >;

template< typename t >
	struct to< t, typename enable_if< is_string< t > >::type >;

template< typename t >
	struct to< t, typename enable_if< component::is_integral< t > >::type >;

template< typename t >
	struct to< t, typename enable_if< component::is_floating_point< t > >::type >;

template< typename t >
	struct to< t, typename enable_if< component::is_pointer< t > >::type >;

template< typename t >
	struct to< t, typename enable_if< component::is_class< t > >::type >;

template< typename t >
	struct to< t, typename enable_if< is_construct< t > >::type >;

template< typename t >
	struct to< t, typename enable_if< is_destruct< t > >::type >;

template< typename t >
	struct to< t, typename enable_if< is_array< t > >::type >;

//--- from -----------------------------------------------------------------------------------------
template< typename, typename = void >
	struct from;

template< typename t >
	struct from< t, typename enable_if< is_empty< t > >::type >;

template< typename t >
	struct from< t, typename enable_if< component::is_boolean< t > >::type >;

template< typename t >
	struct from< t, typename enable_if< is_string< t > >::type >;

template< typename t >
	struct from< t, typename enable_if< component::is_integral< t > >::type >;

template< typename t >
	struct from< t, typename enable_if< component::is_floating_point< t > >::type >;

template< typename t >
	struct from< t, typename enable_if< component::is_pointer< t > >::type >;

template< typename t >
	struct from< t, typename enable_if< component::is_class< t > >::type >;

template< typename t >
	struct from< t, typename enable_if< is_construct< t > >::type >;

template< typename t >
	struct from< t, typename enable_if< is_destruct< t > >::type >;

template< typename t >
	struct from< t, typename enable_if< is_array< t > >::type >;

//--- destroy --------------------------------------------------------------------------------------
template< typename t >
	void destroy( v8::Persistent< v8::Value > value, void* )
{
	v8::Object* object = v8::Object::Cast( *value );

	if ( !object->InternalFieldCount() )
		throw error::javascript() << "Object has no internal fields";

	delete ptr_cast< t* >( object->GetPointerFromInternalField( 0 ) );
	value.Dispose();
}

//--- traits: default ------------------------------------------------------------------------------
template< typename NO_SPECIALISATION_DEFINED, typename >
	struct to
{
	static void call( const v8::Handle< v8::Value >&, NO_SPECIALISATION_DEFINED ) OOE_CONST
	{
		BOOST_STATIC_ASSERT( !sizeof( NO_SPECIALISATION_DEFINED ) );
	}
};

template< typename NO_SPECIALISATION_DEFINED, typename >
	struct from
{
	static v8::Handle< v8::Value > call( NO_SPECIALISATION_DEFINED ) OOE_CONST
	{
		BOOST_STATIC_ASSERT( !sizeof( NO_SPECIALISATION_DEFINED ) );
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

		if ( !object->InternalFieldCount() )
			throw error::javascript() << "Object has no internal fields";

		pointer = ptr_cast< t >( object->GetPointerFromInternalField( 0 ) );
	}
};

template< typename t >
	struct from< t, typename enable_if< component::is_pointer< t > >::type >
{
	static v8::Handle< v8::Value > call( typename call_traits< t >::param_type pointer )
	{
		v8::Handle< v8::ObjectTemplate > template_ = v8::ObjectTemplate::New();
		template_->SetInternalFieldCount( 1 );

		v8::Handle< v8::Object > object = template_->NewInstance();
		object->SetPointerInInternalField( 0, ptr_cast( pointer ) );
		return object;
	}
};

//--- traits: class --------------------------------------------------------------------------------
template< typename t >
	struct to< t, typename enable_if< component::is_class< t > >::type >
{
	static void call( const v8::Handle< v8::Value >& value,
		typename call_traits< t >::reference class_ )
	{
		typedef typename no_ref< t >::type* pointer;
		pointer p;
		to< pointer >::call( value, p );
		class_ = *p;
	}
};

template< typename t >
	struct from< t, typename enable_if< component::is_class< t > >::type >
{
	static v8::Handle< v8::Value > call( typename call_traits< t >::param_type class_ )
	{
		typedef typename no_ref< t >::type type;
		return from< construct_ptr< type > >::call( new type( class_ ) );
	}
};

//--- traits: construct ----------------------------------------------------------------------------
template< typename INVALID_USAGE >
	struct to< INVALID_USAGE, typename enable_if< is_construct< INVALID_USAGE > >::type >
{
	static void call( const v8::Handle< v8::Value >& value, INVALID_USAGE ) OOE_CONST
	{
		BOOST_STATIC_ASSERT( !sizeof( INVALID_USAGE ) );
	}
};

template< typename t >
	struct from< t, typename enable_if< is_construct< t > >::type >
{
	static v8::Handle< v8::Value > call( typename call_traits< t >::param_type construct )
	{
		typedef typename t::value_type type;
		v8::Handle< v8::ObjectTemplate > template_ = v8::ObjectTemplate::New();
		template_->SetInternalFieldCount( 1 );

		v8::Persistent< v8::Object > object =
			v8::Persistent< v8::Object >::New( template_->NewInstance() );
		object->SetPointerInInternalField( 0, construct );
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
		typedef typename t::value_type type;
		pointer p;
		to< pointer >::call( value, p );
		destruct = p;

		v8::Object::Cast( *value )->SetPointerInInternalField( 0, 0 );
	}
};

template< typename INVALID_USAGE >
	struct from< INVALID_USAGE, typename enable_if< is_destruct< INVALID_USAGE > >::type >
{
	static v8::Handle< v8::Value > call( INVALID_USAGE )
	{
		BOOST_STATIC_ASSERT( !sizeof( INVALID_USAGE ) );
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

#endif	// OOE_COMPONENT_JAVASCRIPT_TRAITS_FORWARD_HPP
