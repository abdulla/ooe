/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_COMPONENT_LUA_INVOKE_HPP
	#define OOE_COMPONENT_LUA_INVOKE_HPP

namespace ooe
{
	namespace lua
	{
		template< typename >
			struct invoke_function;

		template< typename, typename >
			struct invoke_member;
	}
}

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "component/lua/function.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	#endif	// OOE_COMPONENT_LUA_INVOKE_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()
	#define COMMA BOOST_PP_COMMA_IF( BOOST_PP_GREATER( LIMIT, 1 ) )

	#define TO( z, n, _ )\
		typename no_ref< t ## n >::type a ## n;\
		to< t ## n >::call( stack, a ## n, n + 1 );

namespace ooe
{
	namespace lua
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

//--- lua::invoke_function -----------------------------------------------------
	template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
		struct lua::invoke_function< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
		static s32 dispatch( lua::state* state )
		{
			stack stack( state );

			typedef void ( * function_type )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
			function_type function;
			to< function_type >::call( stack, function, upvalue( 1 ) );

			BOOST_PP_REPEAT( LIMIT, TO, ~ )
			function( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
		}
	};

	template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct lua::invoke_function< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
		static s32 dispatch( lua::state* state )
		{
			stack stack( state );

			typedef void ( * function_type )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
			function_type function;
			to< function_type >::call( stack, function, upvalue( 1 ) );

			BOOST_PP_REPEAT( LIMIT, TO, ~ )
			r value = function( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
			push< r >::call( stack, value );
		}
	};

#if LIMIT
//--- lua::invoke_member -------------------------------------------------------
	template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
		struct lua::invoke_member< t0, void ( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) ) >
	{
		static s32 dispatch( lua::state* state )
		{
			stack stack( state );

			typedef void ( t0::* member_type )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) );
			member_type member;
			to< member_type >::call( stack, member, upvalue( 1 ) );

			t0* a0;
			to< t0 >::call( stack, a0, 1 );
			BOOST_PP_REPEAT_FROM_TO( 1, LIMIT, TO, ~ )
			( a0->*member )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, a ) );
		}
	};

	template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct lua::invoke_member< t0, r ( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) ) >
	{
		static s32 dispatch( lua::state* state )
		{
			stack stack( state );

			typedef void ( t0::* member_type )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) );
			member_type member;
			to< member_type >::call( stack, member, upvalue( 1 ) );

			t0* a0;
			to< t0 >::call( stack, a0, 1 );
			BOOST_PP_REPEAT_FROM_TO( 1, LIMIT, TO, ~ )
			r value = ( a0->*member )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, a ) );
			push< r >::call( stack, value );
		}
	};
#endif
}

	#undef TO
	#undef COMMA
	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
