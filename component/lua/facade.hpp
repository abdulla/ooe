/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_COMPONENT_LUA_FACADE_HPP
	#define OOE_COMPONENT_LUA_FACADE_HPP

#include <vector>

#include "component/lua/traits.hpp"

namespace ooe
{
	namespace facade
	{
		class lua;
	}

	namespace lua
	{
		void component_setup( stack, const std::string& ) OOE_VISIBLE;
		inline stack verify_arguments( state*, u32 );

		template< typename >
			struct invoke_function;

		template< typename, typename >
			struct invoke_member;
	}

//--- facade::lua --------------------------------------------------------------
	class facade::lua
	{
	public:
		typedef std::vector< ooe::lua::cfunction > vector_type;

		const vector_type& get( void ) const OOE_VISIBLE;
		void insert( up_t, ooe::lua::cfunction ) OOE_VISIBLE;

		template< typename type >
			void insert( up_t index,
			typename enable_if< is_function_pointer< type > >::type* = 0 )
		{
			typedef typename remove_pointer< type >::type function_type;
			insert( index, ooe::lua::invoke_function< function_type >::call );
		}

		template< typename type >
			void insert( up_t index,
			typename enable_if< is_member_function_pointer< type > >::type* = 0 )
		{
			typedef typename member_of< type >::type object_type;
			typedef typename remove_member< type >::type member_type;
			insert( index, ooe::lua::invoke_member< object_type, member_type >::call );
		}

	private:
		vector_type vector;
	};

//--- lua ----------------------------------------------------------------------
	inline lua::stack lua::verify_arguments( state* state, u32 size )
	{
		stack stack( state );
		u32 stack_size = stack.size();

		if ( stack_size < size )
			throw error::lua() << "Not enough arguments to function, " << size <<
				" expected, got " << stack_size;

		return stack;
	}
}

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "component/lua/facade.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	#endif	// OOE_COMPONENT_LUA_FACADE_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()

	#define TO( z, n, _ )\
		typename no_ref< t ## n >::type a ## n;\
		to< typename no_ref< t ## n >::type >::call( stack, a ## n, n + 1 );

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
		static s32 call( state* state )
		{
			stack stack = verify_arguments( state, LIMIT );

			typedef void ( * function_type )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
			function_type function;
			to< function_type >::call( stack, function, upvalue( 1 ) );

			BOOST_PP_REPEAT( LIMIT, TO, ~ )
			function( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );

			return 0;
		}
	};

	template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct lua::invoke_function< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
		static s32 call( state* state )
		{
			stack stack = verify_arguments( state, LIMIT );

			typedef r ( * function_type )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
			function_type function;
			to< function_type >::call( stack, function, upvalue( 1 ) );

			BOOST_PP_REPEAT( LIMIT, TO, ~ )
			r value = function( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
			push< r >::call( stack, value );

			return 1;
		}
	};

#if LIMIT
//--- lua::invoke_member -------------------------------------------------------
	template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
		struct lua::invoke_member< t0, void ( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) ) >
	{
		static s32 call( state* state )
		{
			stack stack = verify_arguments( state, LIMIT );

			typedef void ( t0::* member_type )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) );
			member_type member;
			to< member_type >::call( stack, member, upvalue( 1 ) );

			t0* a0;
			to< t0* >::call( stack, a0, 1 );
			BOOST_PP_REPEAT_FROM_TO( 1, LIMIT, TO, ~ )
			( a0->*member )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, a ) );

			return 0;
		}
	};

	template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct lua::invoke_member< t0, r ( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) ) >
	{
		static s32 call( state* state )
		{
			stack stack = verify_arguments( state, LIMIT );

			typedef r ( t0::* member_type )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) );
			member_type member;
			to< member_type >::call( stack, member, upvalue( 1 ) );

			t0* a0;
			to< t0* >::call( stack, a0, 1 );
			BOOST_PP_REPEAT_FROM_TO( 1, LIMIT, TO, ~ )
			r value = ( a0->*member )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, a ) );
			push< r >::call( stack, value );

			return 1;
		}
	};
#endif
}

	#undef TO
	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
