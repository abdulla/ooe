/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_COMPONENT_LUA_TRAITS_HPP
	#define OOE_COMPONENT_LUA_TRAITS_HPP

#include "component/lua/traits_forward.hpp"
#include "foundation/utility/tuple.hpp"

namespace ooe
{
	namespace lua
	{
//--- lua::to ------------------------------------------------------------------
		template< typename t >
			struct to< t, typename enable_if< is_sequence< t > >::type >;

		template< typename t >
			struct to< t, typename enable_if< is_set< t > >::type >;

		template< typename t >
			struct to< t, typename enable_if< is_map< t > >::type >;

		template< typename t >
			struct to< t, typename enable_if< is_pair< t > >::type >;

//--- lua::push ----------------------------------------------------------------
		template< typename t >
			struct push< t, typename enable_if< is_sequence< t > >::type >;

		template< typename t >
			struct push< t, typename enable_if< is_set< t > >::type >;

		template< typename t >
			struct push< t, typename enable_if< is_map< t > >::type >;

		template< typename t >
			struct push< t, typename enable_if< is_pair< t > >::type >;
	}

//--- lua::traits: sequence ----------------------------------------------------
	template< typename t >
		struct lua::to< t, typename enable_if< is_sequence< t > >::type >
	{
		static void call( stack& stack, typename call_traits< t >::reference container, s32 index )
		{
			type_check( stack, index, typeid( t ), type::table );

			typedef typename no_ref< t >::type type;
			up_t table_size = stack.objlen( index );

			type out;
			reserve( out, table_size );

			for ( up_t i = 0; i != table_size; ++i )
			{
				stack.raw_geti( index, i + 1 );

				typename type::value_type element;
				to< typename type::value_type >::call( stack, element, -1 );
				out.push_back( element );

				stack.pop( 1 );
			}

			container.swap( out );
		}
	};

	template< typename t >
		struct lua::push< t, typename enable_if< is_sequence< t > >::type >
	{
		static void call( stack& stack, typename call_traits< t >::param_type container )
		{
			typedef typename no_ref< t >::type type;
			stack.create_table( container.size() );
			up_t index = 1;

			for ( typename type::const_iterator i = container.begin(), end = container.end();
				i != end; ++i, ++index )
			{
				push< typename type::value_type >::call( stack, *i );
				stack.raw_seti( -2, index );
			}
		}
	};

//--- lua::traits: set ---------------------------------------------------------
	template< typename t >
		struct lua::to< t, typename enable_if< is_set< t > >::type >
	{
		static void call( stack& stack, typename call_traits< t >::reference set, s32 index )
		{
			type_check( stack, index, typeid( t ), type::table );

			typedef typename no_ref< t >::type type;
			type out;
			stack.push_nil();

			for ( ; stack.next( index ); stack.pop( 1 ) )
			{
				typename type::key_type key;
				to< typename type::key_type >::call( stack, key, -2 );
				out.insert( key );
			}

			set.swap( out );
		}
	};

	template< typename t >
		struct lua::push< t, typename enable_if< is_set< t > >::type >
	{
		static void call( stack& stack, typename call_traits< t >::param_type set )
		{
			typedef typename no_ref< t >::type type;
			stack.create_table( 0, set.size() );

			for ( typename type::const_iterator i = set.begin(), end = set.end(); i != end; ++i )
			{
				push< typename type::key_type >::call( stack, *i );
				push< bool >::call( stack, true );
				stack.raw_set( -3 );
			}
		}
	};

//--- lua::traits: map ---------------------------------------------------------
	template< typename t >
		struct lua::to< t, typename enable_if< is_map< t > >::type >
	{
		static void call( stack& stack, typename call_traits< t >::reference map, s32 index )
		{
			type_check( stack, index, typeid( t ), type::table );

			typedef typename no_ref< t >::type type;
			type out;
			stack.push_nil();

			for ( ; stack.next( index ); stack.pop( 1 ) )
			{
				typename type::key_type key;
				to< typename type::key_type >::call( stack, key, -2 );
				typename type::mapped_type mapped;
				to< typename type::mapped_type >::call( stack, mapped, -1 );
				out.insert( typename type::value_type( key, mapped ) );
			}

			map.swap( out );
		}
	};

	template< typename t >
		struct lua::push< t, typename enable_if< is_map< t > >::type >
	{
		static void call( stack& stack, typename call_traits< t >::param_type map )
		{
			typedef typename no_ref< t >::type type;
			stack.create_table( 0, map.size() );

			for ( typename type::const_iterator i = map.begin(), end = map.end(); i != end; ++i )
			{
				push< typename type::key_type >::call( stack, i->first );
				push< typename type::mapped_type >::call( stack, i->second );
				stack.raw_set( -3 );
			}
		}
	};

//--- lua::traits: pair --------------------------------------------------------
	template< typename t >
		struct lua::to< t, typename enable_if< is_pair< t > >::type >
	{
		static void call( stack& stack, typename call_traits< t >::reference pair, s32 index )
		{
			type_check( stack, index, typeid( t ), type::table );
			up_t table_size = stack.objlen( index );

			if ( table_size != 2 )
				throw error::lua() << "Table is of size " << table_size << ", pair is of size 2";

			typedef typename no_ref< t >::type type;

			stack.raw_geti( index, 1 );
			to< typename type::first_type >::call( stack, pair.first, -1 );

			stack.raw_geti( index, 2 );
			to< typename type::second_type >::call( stack, pair.second, -1 );

			stack.pop( 2 );
		}
	};

	template< typename t >
		struct lua::push< t, typename enable_if< is_pair< t > >::type >
	{
		static void call( stack& stack, typename call_traits< t >::param_type pair )
		{
			typedef typename no_ref< t >::type type;
			stack.create_table( 2 );

			push< typename type::first_type >::call( stack, pair.first );
			stack.raw_seti( -2, 1 );

			push< typename type::second_type >::call( stack, pair.second );
			stack.raw_seti( -2, 2 );
		}
	};
}

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "component/lua/traits.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	#endif	// OOE_COMPONENT_LUA_TRAITS_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()

	#define TUPLE_TO( z, n, d )\
		stack.raw_geti( index, n + 1 );\
		to< typename tuple_element< n, t >::type >::call( stack, tuple._ ## n, -1 );

	#define TUPLE_PUSH( z, n, d )\
		push< typename tuple_element< n, t >::type >::call( stack, tuple._ ## n );\
		stack.raw_seti( -2, n + 1 );

namespace ooe
{
	namespace lua
	{
		template< typename t >
			struct to< t, typename enable_if_c< tuple_size< t >::value == LIMIT >::type >;

		template< typename t >
			struct push< t, typename enable_if_c< tuple_size< t >::value == LIMIT >::type >;
	}

//--- lua::traits: tuple -------------------------------------------------------
	template< typename t >
		struct lua::to< t, typename enable_if_c< tuple_size< t >::value == LIMIT >::type >
	{
		static void call( stack& stack, typename call_traits< t >::reference tuple, s32 index )
		{
			type_check( stack, index, typeid( t ), type::table );
			up_t table_size = stack.objlen( index );

			if ( table_size != LIMIT )
				throw error::lua() <<
					"Table is of size " << table_size << ", tuple is of size " << LIMIT;

			BOOST_PP_REPEAT( LIMIT, TUPLE_TO, ~ )
			BOOST_PP_EXPR_IF( LIMIT, stack.pop( LIMIT ); )
		}
	};

	template< typename t >
		struct lua::push< t, typename enable_if_c< tuple_size< t >::value == LIMIT >::type >
	{
		static void call( stack& stack, typename call_traits< t >::param_type tuple )
		{
			stack.create_table( LIMIT );
			BOOST_PP_REPEAT( LIMIT, TUPLE_PUSH, ~ )
		}
	};
}

	#undef TUPLE_PUSH
	#undef TUPLE_TO
	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
