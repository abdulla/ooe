/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_COMPONENT_JAVASCRIPT_TRAITS_HPP
	#define OOE_COMPONENT_JAVASCRIPT_TRAITS_HPP

#include "component/javascript/traits_forward.hpp"
#include "foundation/utility/tuple.hpp"

namespace ooe
{
	namespace javascript
	{
//--- javascript::to -----------------------------------------------------------
		template< typename t >
			struct to< t, typename enable_if< is_stdcontainer< t > >::type >;

		template< typename t >
			struct to< t, typename enable_if< is_set< t > >::type >;

		template< typename t >
			struct to< t, typename enable_if< is_map< t > >::type >;

		template< typename t >
			struct to< t, typename enable_if< is_pair< t > >::type >;

//--- javascript::from ---------------------------------------------------------
		template< typename t >
			struct from< t, typename enable_if< is_stdcontainer< t > >::type >;

		template< typename t >
			struct from< t, typename enable_if< is_set< t > >::type >;

		template< typename t >
			struct from< t, typename enable_if< is_map< t > >::type >;

		template< typename t >
			struct from< t, typename enable_if< is_pair< t > >::type >;
	}

//--- javascript::traits: container --------------------------------------------
	template< typename t >
		struct javascript::to< t, typename enable_if< is_stdcontainer< t > >::type >
	{
		static void call( const v8::Handle< v8::Value >& value,
			typename call_traits< t >::reference container )
		{
			if ( !value->IsArray() )
				throw error::javascript() << "Value is not an array";

			typedef typename no_ref< t >::type type;
			v8::Array* array = v8::Array::Cast( *value );
			up_t array_size = array->Length();

			type out;
			reserve( out, array_size );
			std::insert_iterator< type > j( out, out.begin() );

			v8::HandleScope scope;

			for ( up_t i = 0; i != array_size; ++i, ++j )
			{
				v8::Local< v8::Value > item = array->Get( from< up_t >::call( i ) );

				typename type::value_type element;
				to< typename type::value_type >::call( item, element );
				*j = element;
			}

			container.swap( out );
		}
	};

	template< typename t >
		struct javascript::from< t, typename enable_if< is_stdcontainer< t > >::type >
	{
		static v8::Handle< v8::Value > call( typename call_traits< t >::param_type container )
		{
			typedef typename no_ref< t >::type type;
			v8::Local< v8::Array > local = v8::Array::New( container.size() );
			up_t index = 0;

			for ( typename type::const_iterator i = container.begin(), end = container.end();
				i != end; ++i, ++index )
				local->Set( v8::Number::New( index ),
					from< typename type::value_type >::call( *i ) );

			return local;
		}
	};

//--- javascript::traits: set --------------------------------------------------
//--- javascript::traits: map --------------------------------------------------
//--- javascript::traits: pair -------------------------------------------------
	template< typename t >
		struct javascript::to< t, typename enable_if< is_pair< t > >::type >
	{
		static void call( const v8::Handle< v8::Value >& value,
			typename call_traits< t >::reference pair )
		{
			if ( !value->IsArray() )
				throw error::javascript() << "Value is not an array";

			v8::Array* array = v8::Array::Cast( *value );
			up_t array_size = array->Length();

			if ( array_size != 2 )
				throw error::javascript() <<
					"Array is of size " << array_size << ", pair is of size 2";

			typedef typename no_ref< t >::type type;
			v8::HandleScope scope;

			to< typename type::first_type >::
				call( array->Get( from< up_t >::call( 0 ) ), pair.first );
			to< typename type::second_type >::
				call( array->Get( from< up_t >::call( 1 ) ), pair.second );
		}
	};

	template< typename t >
		struct javascript::from< t, typename enable_if< is_pair< t > >::type >
	{
		static v8::Handle< v8::Value > call( typename call_traits< t >::param_type pair )
		{
			typedef typename no_ref< t >::type type;
			v8::Local< v8::Array > local = v8::Array::New( 2 );

			local->Set( from< up_t >::call( 0 ),
				from< typename type::first_type >::call( pair.first ) );
			local->Set( from< up_t >::call( 1 ),
				from< typename type::second_type >::call( pair.second ) );

			return local;
		}
	};
}

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "component/javascript/traits.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	#endif	// OOE_COMPONENT_JAVASCRIPT_TRAITS_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()

	#define TUPLE_TO( z, n, d )\
		to< typename tuple_element< n, t >::type >::\
			call( array->Get( from< up_t >::call( n ) ), tuple._ ## n );

	#define TUPLE_FROM( z, n, d )\
		local->Set( from< up_t >::call( n ),\
			from< typename tuple_element< n, t >::type >::call( tuple._ ## n ) );

namespace ooe
{
	namespace javascript
	{
		template< typename t >
			struct to< t, typename enable_if_c< tuple_size< t >::value == LIMIT >::type >;

		template< typename t >
			struct from< t, typename enable_if_c< tuple_size< t >::value == LIMIT >::type >;
	}

//--- javascript::traits:: tuple -----------------------------------------------
	template< typename t >
		struct javascript::to< t, typename enable_if_c< tuple_size< t >::value == LIMIT >::type >
	{
		static void call( const v8::Handle< v8::Value >& value,
			typename call_traits< t >::reference tuple )
		{
			if ( !value->IsArray() )
				throw error::javascript() << "Value is not an array";

			v8::Array* array = v8::Array::Cast( *value );
			up_t array_size = array->Length();

			if ( array_size != LIMIT )
				throw error::javascript() <<
					"Array is of size " << array_size << ", tuple is of size " << LIMIT;

			v8::HandleScope scope;
			BOOST_PP_REPEAT( LIMIT, TUPLE_TO, ~ )
		}
	};

	template< typename t >
		struct javascript::from< t, typename enable_if_c< tuple_size< t >::value == LIMIT >::type >
	{
		static v8::Handle< v8::Value > call( typename call_traits< t >::param_type tuple )
		{
			v8::Local< v8::Array > local = v8::Array::New( LIMIT );
			BOOST_PP_REPEAT( LIMIT, TUPLE_FROM, ~ )
			return local;
		}
	};
}

	#undef TUPLE_FROM
	#undef TUPLE_TO
	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
