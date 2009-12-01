/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_COMPONENT_JAVASCRIPT_FACADE_HPP
	#define OOE_COMPONENT_JAVASCRIPT_FACADE_HPP

#include <vector>

#include "component/javascript/traits.hpp"
#include "component/javascript/vm.hpp"

namespace ooe
{
	namespace facade
	{
		class javascript;
	}

	namespace javascript
	{
		void component_setup( v8::Handle< v8::Object >, const std::string& ) OOE_VISIBLE;
		void throw_exception( const c8*, const c8* ) OOE_VISIBLE;
		inline void verify_arguments( const v8::Arguments&, s32 );

		template< typename >
			struct invoke;

		template< typename >
			struct invoke_function;

		template< typename, typename >
			struct invoke_member;
	}

//--- facade::javascript -------------------------------------------------------
	class facade::javascript
	{
	public:
		typedef std::vector< v8::InvocationCallback > vector_type;

		const vector_type& get( void ) const OOE_VISIBLE;
		void insert( up_t, v8::InvocationCallback ) OOE_VISIBLE;

		template< typename type >
			void insert( up_t index,
			typename enable_if< is_function_pointer< type > >::type* = 0 )
		{
			typedef typename remove_pointer< type >::type function_type;
			typedef ooe::javascript::
				invoke< ooe::javascript::invoke_function< function_type > > invoke_type;
			insert( index, invoke_type::call );
		}

		template< typename type >
			void insert( up_t index,
			typename enable_if< is_member_function_pointer< type > >::type* = 0 )
		{
			typedef typename member_of< type >::type object_type;
			typedef typename remove_member< type >::type member_type;
			typedef ooe::javascript::
				invoke< ooe::javascript::invoke_member< object_type, member_type > > invoke_type;
			insert( index, invoke_type::call );
		}

	private:
		vector_type vector;
	};

//--- javascript ---------------------------------------------------------------
	inline void javascript::verify_arguments( const v8::Arguments& arguments, s32 size )
	{
		s32 argument_size = arguments.Length();

		if ( argument_size < size )
			throw error::javascript() << "Not enough arguments to function, " << size <<
				" expected, got " << argument_size;
	}

//--- javascript::invoke -------------------------------------------------------
	template< typename type >
		struct javascript::invoke
	{
		static v8::Handle< v8::Value > call( const v8::Arguments& arguments )
		{
			v8::HandleScope scope;

			try
			{
				return type::call( arguments );
			}
			catch ( error::runtime& error )
			{
				throw_exception( error.what(), error.where() );
			}
			catch ( std::exception& error )
			{
				throw_exception( error.what(), "\nNo stack trace available" );
			}
			catch ( ... )
			{
				throw_exception( "An unknown exception was thrown", "\nNo stack trace available" );
			}

			return v8::Undefined();
		}
	};
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

			typedef void ( * function_type )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
			function_type function;
			to< function_type >::call( arguments.Data(), function );

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

			typedef r ( * function_type )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
			function_type function;
			to< function_type >::call( arguments.Data(), function );

			BOOST_PP_REPEAT( LIMIT, TO, ~ )
			r value = function( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
			return from< r >::call( value );
		}
	};

#if LIMIT
//--- javascript::invoke_member ------------------------------------------------
	template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
		struct javascript::invoke_member< t0, void ( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) ) >
	{
		static v8::Handle< v8::Value > call( const v8::Arguments& arguments )
		{
			verify_arguments( arguments, LIMIT );

			typedef void ( t0::* member_type )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) );
			member_type member;
			to< member_type >::call( arguments.Data(), member );

			t0* a0;
			to< t0* >::call( arguments[ 0 ], a0 );
			BOOST_PP_REPEAT_FROM_TO( 1, LIMIT, TO, ~ )
			( a0->*member )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, a ) );
			return v8::Undefined();
		}
	};

	template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct javascript::invoke_member< t0, r ( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) ) >
	{
		static v8::Handle< v8::Value > call( const v8::Arguments& arguments )
		{
			verify_arguments( arguments, LIMIT );

			typedef void ( t0::* member_type )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) );
			member_type member;
			to< member_type >::call( arguments.Data(), member );

			t0* a0;
			to< t0* >::call( arguments[ 0 ], a0 );
			BOOST_PP_REPEAT_FROM_TO( 1, LIMIT, TO, ~ )
			r value = ( a0->*member )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, a ) );
			return from< r >::call( value );
		}
	};
#endif
}

	#undef TO
	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
