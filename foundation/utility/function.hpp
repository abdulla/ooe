/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_FOUNDATION_UTILITY_FUNCTION_HPP
	#define OOE_FOUNDATION_UTILITY_FUNCTION_HPP

#include "foundation/utility/miscellany.hpp"

namespace ooe
{
	template< typename >
		struct invoke_f;	// function

	template< typename, typename >
		struct invoke_o;	// object

	template< typename, typename >
		struct invoke_c;	// const object

	template< typename, typename >
		struct invoke_p;	// object + function

	template< typename, typename >
		struct invoke_d;	// const object + function

	template< typename >
		struct invoke_n;	// null

	template< typename >
		struct function;

//--- make_function ------------------------------------------------------------
	template< typename type, typename pointer >
		function< typename remove_callable< pointer >::type > make_function( type& t, pointer p )
	{
		return function< typename remove_callable< pointer >::type >( t, p );
	}

	template< typename pointer >
		function< typename remove_callable< pointer >::type > make_function( pointer p )
	{
		return function< typename remove_callable< pointer >::type >( p );
	}
}

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "foundation/utility/function.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	#endif	// OOE_FOUNDATION_UTILITY_FUNCTION_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()

namespace ooe
{
//--- invoke_f -----------------------------------------------------------------
	template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct invoke_f< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
		static r call( const bound_any& any BOOST_PP_ENUM_TRAILING_BINARY_PARAMS
			( LIMIT, typename call_traits< t, >::param_type a ) )
		{
			return reinterpret_cast< r ( * )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >( any.function )
				( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
		}
	};

//--- invoke_o -----------------------------------------------------------------
	template< typename type, typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct invoke_o< type, r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
		static r call( const bound_any& any BOOST_PP_ENUM_TRAILING_BINARY_PARAMS
			( LIMIT, typename call_traits< t, >::param_type a ) )
		{
			return ( static_cast< type* >( any.object.pointer )->*
				reinterpret_cast< r ( type::* )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
				( any.object.member ) )( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
		}
	};

//--- invoke_c -----------------------------------------------------------------
	template< typename type, typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct invoke_c< type, r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
		static r call( const bound_any& any BOOST_PP_ENUM_TRAILING_BINARY_PARAMS
			( LIMIT, typename call_traits< t, >::param_type a ) )
		{
			return ( static_cast< const type* >( any.object.pointer )->*
				reinterpret_cast< r ( type::* )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
				( any.object.member ) )( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
		}
	};

//--- invoke_p -----------------------------------------------------------------
	template< typename type, typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct invoke_p< type, r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
		static r call( const bound_any& any BOOST_PP_ENUM_TRAILING_BINARY_PARAMS
			( LIMIT, typename call_traits< t, >::param_type a ) )
		{
			return reinterpret_cast< r ( * )( type& BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, t ) ) >
				( any.object.function )( *static_cast< type* >( any.object.pointer )
				BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );
		}
	};

//--- invoke_d -----------------------------------------------------------------
	template< typename type, typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct invoke_d< type, r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
		static r call( const bound_any& any BOOST_PP_ENUM_TRAILING_BINARY_PARAMS
			( LIMIT, typename call_traits< t, >::param_type a ) )
		{
			return reinterpret_cast< r ( * )
				( const type& BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, t ) ) >( any.object.function )
				( *static_cast< const type* >( any.object.pointer )
				BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );
		}
	};

//--- invoke_n -----------------------------------------------------------------
	template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct invoke_n< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
		static r call( const bound_any& BOOST_PP_ENUM_TRAILING_BINARY_PARAMS
			( LIMIT, typename call_traits< t, >::param_type BOOST_PP_INTERCEPT ) )
		{
			return r();
		}
	};

//--- function -----------------------------------------------------------------
	template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		class function< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
	public:
		function( void )
			: call( invoke_n< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >::call ), pointer( 0 )
		{
		}

		function( r ( * function_ )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) )
			: call( invoke_f< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >::call ), pointer( function_ )
		{
		}

		template< typename type >
			function( type& object, r ( type::* member )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) )
			: call( invoke_o< type, r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >::call ),
			pointer( bound_any( &object, member ) )
		{
		}

		template< typename type >
			function( type& object, r ( type::* member )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) const )
			: call( invoke_o< type, r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >::call ),
			pointer( bound_any( &object,
				reinterpret_cast< r ( type::* )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >( member ) ) )
		{
		}

		template< typename type >
			function( const type& object, r ( type::* member )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) )
			: call( invoke_c< type, r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >::call ),
			pointer( bound_any( &object, member ) )
		{
		}

		template< typename type >
			function( const type& object,
			r ( type::* member )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) const )
			: call( invoke_c< type, r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >::call ),
			pointer( bound_any( &object,
				reinterpret_cast< r ( type::* )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >( member ) ) )
		{
		}

		template< typename type >
			function( type& object,
			r ( * function_ )( type& BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, t ) ) )
			: call( invoke_p< type, r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >::call ),
			pointer( bound_any( &object, function_ ) )
		{
		}

		template< typename type >
			function( const type& object,
			r ( * function_ )( const type& BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, t ) ) )
			: call( invoke_d< type, r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >::call ),
			pointer( bound_any( &object, function_ ) )
		{
		}

		void clear( void )
		{
			this->call = invoke_n< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >::call;
		}

		r operator ()( BOOST_PP_ENUM_BINARY_PARAMS
			( LIMIT, typename call_traits< t, >::param_type a ) ) const
		{
			return call( pointer BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );
		}

	private:
		typedef r ( * call_type )( const bound_any& BOOST_PP_ENUM_TRAILING_BINARY_PARAMS
			( LIMIT, typename call_traits< t, >::param_type BOOST_PP_INTERCEPT ) );

		call_type call;
		bound_any pointer;
	};
}

	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
