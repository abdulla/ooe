/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_JAVASCRIPT_TRAITS_FORWARD_HPP
#define OOE_COMPONENT_JAVASCRIPT_TRAITS_FORWARD_HPP

#include <v8.h>

#include "component/javascript/error.hpp"
#include "foundation/utility/hint.hpp"
#include "foundation/utility/miscellany.hpp"

namespace ooe
{
	namespace javascript
	{
		template< typename >
			struct is_boolean;

		template< typename >
			struct is_floating_point;

		template< typename >
			struct is_integral;

		template< typename >
			struct is_pointer;

		template< typename >
			struct is_class;

//--- javascript::to -----------------------------------------------------------
		template< typename, typename = void >
			struct to;

		template< typename t >
			struct to< t, typename enable_if< is_empty< t > >::type >;

		template< typename t >
			struct to< t, typename enable_if< is_boolean< t > >::type >;

		template< typename t >
			struct to< t, typename enable_if< is_string< t > >::type >;

		template< typename t >
			struct to< t, typename enable_if< is_floating_point< t > >::type >;

		template< typename t >
			struct to< t, typename enable_if< is_integral< t > >::type >;

		template< typename t >
			struct to< t, typename enable_if< is_pointer< t > >::type >;

		template< typename t >
			struct to< t, typename enable_if< is_class< t > >::type >;

		template< typename t >
			struct to< t, typename enable_if< is_construct< t > >::type >;

		template< typename t >
			struct to< t, typename enable_if< is_destruct< t > >::type >;

		template< typename t >
			struct to< t, typename enable_if< is_array< t > >::type >;

//--- javascript:from ----------------------------------------------------------
		template< typename, typename = void >
			struct from;

		template< typename t >
			struct from< t, typename enable_if< is_empty< t > >::type >;

		template< typename t >
			struct from< t, typename enable_if< is_boolean< t > >::type >;

		template< typename t >
			struct from< t, typename enable_if< is_string< t > >::type >;

		template< typename t >
			struct from< t, typename enable_if< is_floating_point< t > >::type >;

		template< typename t >
			struct from< t, typename enable_if< is_integral< t > >::type >;

		template< typename t >
			struct from< t, typename enable_if< is_pointer< t > >::type >;

		template< typename t >
			struct from< t, typename enable_if< is_class< t > >::type >;

		template< typename t >
			struct from< t, typename enable_if< is_construct< t > >::type >;

		template< typename t >
			struct from< t, typename enable_if< is_destruct< t > >::type >;

		template< typename t >
			struct from< t, typename enable_if< is_array< t > >::type >;

//--- destroy ------------------------------------------------------------------
		template< typename >
			void destroy( v8::Persistent< v8::Value >, void* );
	}

//--- javascript::is_boolean ---------------------------------------------------
	template< typename t >
		struct javascript::is_boolean
		: public is_like< t, bool >
	{
	};

//--- javascript::is_floating_point --------------------------------------------
	template< typename t >
		struct javascript::is_floating_point
		: public ooe::is_floating_point< typename no_ref< t >::type >
	{
	};

//--- javascript::is_integral --------------------------------------------------
	template< typename t >
		struct javascript::is_integral
	{
		static const bool value =
			!is_boolean< t >::value &&
			ooe::is_integral< typename no_ref< t >::type >::value;
	};

//--- javascript::traits: is_pointer -------------------------------------------
	template< typename t >
		struct javascript::is_pointer
	{
		static const bool value =
			!is_cstring< t >::value &&
			!is_member_pointer< typename no_ref< t >::type >::value &&
			ooe::is_pointer< typename no_ref< t >::type >::value;
	};

//--- javascript::traits: is_class ---------------------------------------------
	template< typename t >
		struct javascript::is_class
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

//--- javascript::traits: default ----------------------------------------------
	template< typename NO_SPECIALISATION_DEFINED, typename >
		struct javascript::to
	{
		static void call( const v8::Handle< v8::Value >&, NO_SPECIALISATION_DEFINED )
		{
			BOOST_STATIC_ASSERT( !sizeof( NO_SPECIALISATION_DEFINED ) );
		}
	};

	template< typename NO_SPECIALISATION_DEFINED, typename >
		struct javascript::from
	{
		static v8::Handle< v8::Value > call( NO_SPECIALISATION_DEFINED ) OOE_CONST
		{
			BOOST_STATIC_ASSERT( !sizeof( NO_SPECIALISATION_DEFINED ) );
			return v8::Undefined();
		}
	};

//--- javascript::traits: empty ------------------------------------------------
	template< typename t >
		struct javascript::to< t, typename enable_if< is_empty< t > >::type >
	{
		static void call( const v8::Handle< v8::Value >& value,
			typename call_traits< t >::reference )
		{
			if ( !value->IsNull() )
				throw error::javascript() << "Value is not null";
		}
	};

	template< typename t >
		struct javascript::from< t, typename enable_if< is_empty< t > >::type >
	{
		static v8::Handle< v8::Value > call( typename call_traits< t >::param_type )
		{
			return v8::Null();
		}
	};

//--- javascript::traits: boolean ----------------------------------------------
	template< typename t >
		struct javascript::to< t, typename enable_if< javascript::is_boolean< t > >::type >
	{
		static void call( const v8::Handle< v8::Value >& value, bool& boolean )
		{
			if ( !value->IsBoolean() )
				throw error::javascript() << "Value is not a boolean";

			boolean = value->ToBoolean()->Value();
		}
	};

	template< typename t >
		struct javascript::from< t, typename enable_if< javascript::is_boolean< t > >::type >
	{
		static v8::Handle< v8::Value > call( bool boolean )
		{
			return v8::Boolean::New( boolean );
		}
	};

//--- javascript::traits: string -----------------------------------------------
	template< typename t >
		struct javascript::to< t, typename enable_if< is_string< t > >::type >
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
		struct javascript::from< t, typename enable_if< is_string< t > >::type >
	{
		static v8::Handle< v8::Value > call( typename call_traits< t >::param_type string )
		{
			return v8::String::New( string_data( string ), string_size( string ) );
		}
	};

//--- javascript::traits: floating-point ---------------------------------------
	template< typename t >
		struct javascript::to< t, typename enable_if< javascript::is_floating_point< t > >::type >
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
		struct javascript::from< t, typename enable_if< javascript::is_floating_point< t > >::type >
	{
		static v8::Handle< v8::Value > call( typename call_traits< t >::param_type floating_point )
		{
			return v8::Number::New( floating_point );
		}
	};

//--- javascript::traits: integral ---------------------------------------------
	template< typename t >
		struct javascript::to< t, typename enable_if< javascript::is_integral< t > >::type >
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
		struct javascript::from< t, typename enable_if< javascript::is_integral< t > >::type >
	{
		static v8::Handle< v8::Value > call( typename call_traits< t >::param_type integral )
		{
			return v8::Integer::New( integral );
		}
	};

//--- javascript::traits: pointer ----------------------------------------------
	template< typename t >
		struct javascript::to< t, typename enable_if< javascript::is_pointer< t > >::type >
	{
		static void call( const v8::Handle< v8::Value >& value,
			typename call_traits< t >::reference pointer )
		{
			if ( !value->IsObject() )
				throw error::javascript() << "Value is not an object";

			v8::Object* object = v8::Object::Cast( *value );

			if ( !object->InternalFieldCount() )
				throw error::javascript() << "Object has no internal fields";

			pointer =
				ptr_cast< typename no_ref< t >::type >( object->GetPointerFromInternalField( 0 ) );
		}
	};

	template< typename t >
		struct javascript::from< t, typename enable_if< javascript::is_pointer< t > >::type >
	{
		static v8::Handle< v8::Value > call( typename call_traits< t >::param_type pointer )
		{
			v8::Handle< v8::Object > object = v8::Object::New();
			object->SetPointerInInternalField( 0, ptr_cast( pointer ) );
			return object;
		}
	};

//--- javascript::traits: class ------------------------------------------------
	template< typename t >
		struct javascript::to< t, typename enable_if< javascript::is_class< t > >::type >
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
		struct javascript::from< t, typename enable_if< javascript::is_class< t > >::type >
	{
		static v8::Handle< v8::Value > call( typename call_traits< t >::param_type class_ )
		{
			typedef typename no_ref< t >::type type;
			return from< construct_ptr< type > >::call( new type( class_ ) );
		}
	};

//--- javascript::traits: construct --------------------------------------------
	template< typename t >
		struct javascript::to< t, typename enable_if< is_construct< t > >::type >
	{
		static void call( const v8::Handle< v8::Value >& value,
			typename call_traits< t >::reference construct )
		{
			typedef typename t::pointer pointer;
			pointer p;
			to< pointer >::call( value, p );
			construct = p;
		}
	};

	template< typename t >
		struct javascript::from< t, typename enable_if< is_construct< t > >::type >
	{
		static v8::Handle< v8::Value > call( typename call_traits< t >::param_type construct )
		{
			typedef typename t::value_type type;
			v8::Persistent< v8::Object > object =
				v8::Persistent< v8::Object >::New( v8::Object::New() );
			object->SetPointerInInternalField( 0, construct );
			object.MakeWeak( 0, destroy< type > );
			object.Dispose();
			v8::V8::AdjustAmountOfExternalAllocatedMemory( sizeof( type ) );
			return object;
		}
	};

//--- javascript::traits: destruct ---------------------------------------------
	template< typename t >
		struct javascript::to< t, typename enable_if< is_destruct< t > >::type >
	{
		static void call( const v8::Handle< v8::Value >& value,
			typename call_traits< t >::reference destruct )
		{
			v8::Persistent< v8::Value > persistent( value );

			if ( !persistent.IsWeak() )
				throw error::javascript() << "Value is not a weak reference";

			typedef typename t::pointer pointer;
			pointer p;
			to< pointer >::call( value, p );
			destruct = p;
			persistent.ClearWeak();
		}
	};

	template< typename t >
		struct javascript::from< t, typename enable_if< is_destruct< t > >::type >
	{
		static v8::Handle< v8::Value > call( typename call_traits< t >::param_type destruct )
		{
			return from< typename t::pointer >::call( destruct );
		}
	};

//--- javascript::traits: array ------------------------------------------------
	template< typename t >
		struct javascript::to< t, typename enable_if< is_array< t > >::type >
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
		struct javascript::from< t, typename enable_if< is_array< t > >::type >
	{
		static v8::Handle< v8::Value > call( typename call_traits< t >::param_type array )
		{
			typedef typename no_ref< t >::type type;
			up_t array_size = extent< type >::value;
			v8::Local< v8::Array > local = v8::Array::New( array_size );

			for ( up_t i = 0; i != array_size; ++i )
				local->Set( from< up_t >::call( i ),
					from< typename remove_extent< type >::type >::call( array[ i ] ) );

			return local;
		}
	};

//--- javascript::destroy ------------------------------------------------------
	template< typename type >
		void javascript::destroy( v8::Persistent< v8::Value > value, void* )
	{
		type* pointer;
		to< type* >::call( value, pointer );
		delete pointer;
		value.ClearWeak();
	}
}

#endif	// OOE_COMPONENT_JAVASCRIPT_TRAITS_FORWARD_HPP
