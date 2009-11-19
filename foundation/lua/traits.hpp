/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_FOUNDATION_LUA_TRAITS_HPP
	#define OOE_FOUNDATION_LUA_TRAITS_HPP

#include "foundation/utility/convert.hpp"
#include "foundation/utility/miscellany.hpp"
#include "foundation/utility/traits.hpp"

#include "foundation/lua/error.hpp"
#include "foundation/lua/traits_forward.hpp"
#include "foundation/lua/traits_specialised.hpp"

namespace ooe
{
	namespace lua
	{
		template< typename >
			struct function;

		template< typename type >
			struct is_specialised< function< type > >;

		template< typename type >
			struct traits< function< type >, void >;
	}

//--- lua::is_specialised ------------------------------------------------------
	template< typename type >
		struct lua::is_specialised< lua::function< type > >
		: public true_type
	{
	};

//--- lua::traits< type, lua::function > ---------------------------------------
	template< typename type >
		struct lua::traits< lua::function< type >, void >
	{
		typedef function< type > value_type;

		static void push( stack& stack, const value_type& function )
		{
			stack.rawgeti( registry, function.get() );
		}

		static value_type to( stack& stack, s32 index )
		{
			type_check< value_type >( stack, index, id::function );
			return value_type( stack, index );
		}
	};
}

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "foundation/lua/traits.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	#endif	// OOE_FOUNDATION_LUA_TRAITS_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()
	#define PUSH( z, n, _ ) traits< t ## n >::push( stack, a ## n );
	#define TO( z, n, _ ) traits< t ## n >::to( stack, -BOOST_PP_SUB( LIMIT, n ) )
	#define TUPLE_GET( z, n, _ ) stack.rawgeti( index, BOOST_PP_INC( n ) );

	#define TUPLE_SET( z, n, _ )\
		traits< typename tuple_element< n, value_type >::type >::push( stack, tuple._ ## n );\
		stack.rawseti( -2, BOOST_PP_INC( n ) );

	#define TUPLE_TO( z, n, _ )\
		traits< typename tuple_element< n, value_type >::type >::\
			to( stack, -BOOST_PP_SUB( LIMIT, n ) )

namespace ooe
{
	namespace lua
	{
		template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
			struct function_native< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >;

		template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
			struct function_native< r ( * )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >;

		template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
			struct function_native< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >;

		template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
			struct function_native< void ( * )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >;

		template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
			class function< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >;

		template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
			struct is_specialised< tuple< BOOST_PP_ENUM_PARAMS( LIMIT, t ) > >;

		template< typename type >
			struct traits< type, typename enable_if_c< tuple_size< type >::value == LIMIT >::type >;
	}

//--- lua::function_native -----------------------------------------------------
	template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct lua::function_native< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
		static s32 dispatch( lua::state* state )
		{
			stack stack( state );

			void* userdata = stack.touserdata( upvalue( 1 ) );
			typedef r ( * type )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
			type call = ptr_cast< type >( userdata );

			OOE_ERROR( traits< r >::push( stack, call( BOOST_PP_ENUM( LIMIT, TO, ~ ) ) ) );
			return 1;
		}
	};

	template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct lua::function_native< r ( * )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
		: public function_native< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
	};

	template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
		struct lua::function_native< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
		static s32 dispatch( lua::state* state )
		{
			stack stack( state );

			void* userdata = stack.touserdata( upvalue( 1 ) );
			typedef void ( * type )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
			type call = ptr_cast< type >( userdata );

			OOE_ERROR( call( BOOST_PP_ENUM( LIMIT, TO, ~ ) ) );
			return 0;
		}
	};

	template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
		struct lua::function_native< void ( * )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
		: public function_native< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
	};

//--- lua::function ------------------------------------------------------------
	template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		class lua::function< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
	public:
		function( lua::stack& stack_, s32 index )
			: stack( stack_ ), ref()
		{
			stack.pushvalue( index );
			ref = stack.ref();
		}

		~function( void )
		{
			stack.unref( ref );
		}

		r operator ()( BOOST_PP_ENUM_BINARY_PARAMS( LIMIT, t, a ) )
		{
			stack.rawgeti( registry, ref );
			BOOST_PP_REPEAT( LIMIT, PUSH, ~ )
			stack.pcall( LIMIT, !is_same< r, void >::value );

			return traits< r >::to( stack, -1 );
		}

		s32 get( void ) const
		{
			return ref;
		}

	private:
		lua::stack stack;
		s32 ref;
	};

//--- lua::is_specialised ------------------------------------------------------
	template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
		struct lua::is_specialised< tuple< BOOST_PP_ENUM_PARAMS( LIMIT, t ) > >
		: public true_type
	{
	};

//--- lua::traits< type, tuple > -----------------------------------------------
	template< typename type >
		struct lua::traits< type, typename enable_if_c< tuple_size< type >::value == LIMIT >::type >
	{
		typedef typename no_ref< type >::type value_type;

		// push - converts c++ tuple to table in lua
		static void push( stack& stack, const value_type& BOOST_PP_EXPR_IF( LIMIT, tuple ) )
		{
			stack.createtable( LIMIT, 0 );
			BOOST_PP_REPEAT( LIMIT, TUPLE_SET, _ )
		}

		// to - converts table in lua to c++ tuple
		static value_type to( stack& stack, s32 index )
		{
			type_check< value_type >( stack, index, id::table );
			BOOST_PP_REPEAT( LIMIT, TUPLE_GET, ~ )
			value_type BOOST_PP_IF( LIMIT, t( BOOST_PP_ENUM( LIMIT, TUPLE_TO, ~ ) ), t );
			BOOST_PP_EXPR_IF( LIMIT, stack.pop( LIMIT ) );
			return t;
		}
	};
}

	#undef TUPLE_TO
	#undef TUPLE_SET
	#undef TUPLE_GET
	#undef TO
	#undef PUSH
	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
