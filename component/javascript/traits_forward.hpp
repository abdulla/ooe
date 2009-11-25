/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_JAVASCRIPT_TRAITS_FORWARD_HPP
#define OOE_COMPONENT_JAVASCRIPT_TRAITS_FORWARD_HPP

#include <v8.h>

#include "component/javascript/error.hpp"
#include "foundation/utility/traits.hpp"

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

//--- javascript::to -----------------------------------------------------------
		template< typename, typename = void >
			struct to;

		template< typename t >
			struct to< t, typename enable_if< is_boolean< t > >::type >;

		template< typename t >
			struct to< t, typename enable_if< is_string< t > >::type >;

		template< typename t >
			struct to< t, typename enable_if< is_floating_point< t > >::type >;

		template< typename t >
			struct to< t, typename enable_if< is_integral< t > >::type >;

		template< typename t >
			struct to< t, typename enable_if< is_array< t > >::type >;

//--- javascript:from ----------------------------------------------------------
		template< typename, typename = void >
			struct from;

		template< typename t >
			struct from< t, typename enable_if< is_boolean< t > >::type >;

		template< typename t >
			struct from< t, typename enable_if< is_string< t > >::type >;

		template< typename t >
			struct from< t, typename enable_if< is_floating_point< t > >::type >;

		template< typename t >
			struct from< t, typename enable_if< is_integral< t > >::type >;

		template< typename t >
			struct from< t, typename enable_if< is_array< t > >::type >;
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
}

#endif	// OOE_COMPONENT_JAVASCRIPT_TRAITS_FORWARD_HPP
