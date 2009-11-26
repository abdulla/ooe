/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_COMPONENT_JAVASCRIPT_FACADE_HPP
	#define OOE_COMPONENT_JAVASCRIPT_FACADE_HPP

#include <vector>

#include "component/javascript/traits.hpp"

namespace ooe
{
	namespace facade
	{
		class javascript;
	}

	namespace javascript
	{
		inline void verify_arguments( const v8::Arguments&, s32 );
		inline v8::Local< v8::Value > verify_external( const v8::Arguments& );

		template< typename >
			struct invoke_function;

		template< typename, typename >
			struct invoke_member;
	}

//--- facade::lua --------------------------------------------------------------
	class facade::lua
	{
	public:
		typedef std::vector< v8::InvocationCallback > vector_type;

		const vector_type& get( void ) const OOE_VISIBLE;
		void insert( up_t, v8::InvocationCallback ) OOE_VISIBLE;

	private:
		vector_type vector;
	};

//--- javascript ---------------------------------------------------------------
	inline void verify_arguments( const v8::Arguments& arguments, s32 size )
	{
		s32 argument_size = arguments.Length();

		if ( argument_size < size )
			throw error::javascript() << "Not enough arguments to function, " << size <<
				" expected, got " << argument_size;
	}

	inline v8::Local< v8::Value > verify_external( const v8::Arguments& arguments )
	{
		v8::Local< v8::Value > external = arguments.Data();

		if ( !external->IsExternal() )
			throw error::javascript() << "Value is not an external pointer";

		return external;
	}
}

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "component/javascript/facade.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	#endif	// OOE_COMPONENT_JAVASCRIPT_FACADE_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()

	#define TO( z, n, _ )\
		typename no_ref< t ## n >::type a ## n;\
		to< typename no_ref< t ## n >::type >::call( arguments[ n ], a ## n );

namespace ooe
{
	namespace javascript
	{
		template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
			struct invoke_function< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >;

		template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
			struct invoke_function< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >;

#if LIMIT
		template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
			struct invoke_member< t0, void ( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) ) >;

		template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
			struct invoke_member< t0, r ( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) ) >;
#endif
	}

//--- javascript::invoke_function ----------------------------------------------
	template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
		struct javascript::invoke_function< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
		static v8::Handle< v8::Value > call( const v8::Arguments& arguments )
		{
			verify_arguments( arguments, LIMIT );
			v8::HandleScope scope;
			v8::Local< v8::Value > external = verify_external( arguments );

			typedef void ( * function_type )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
			function_type function =
				*static_cast< function_type* >( v8::External::Unwrap( *external ) );

			BOOST_PP_REPEAT( LIMIT, TO, ~ )
			function( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
			return v8::Undefined();
		}
	};

	template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct javascript::invoke_function< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
		static v8::Handle< v8::Value > call( const v8::Arguments& arguments )
		{
			verify_arguments( arguments, LIMIT );
			v8::HandleScope scope;
			v8::Local< v8::Value > external = verify_external( arguments );

			typedef r ( * function_type )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
			function_type function =
				*static_cast< function_type* >( v8::External::Unwrap( *external ) );

			BOOST_PP_REPEAT( LIMIT, TO, ~ )
			r value = function( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
			return from< r >::call( value );
		}
	};

//--- javascript::invoke_member ------------------------------------------------
}

	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
