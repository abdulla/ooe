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
		template< typename t >
			struct to< t, typename enable_if< is_stdcontainer< t > >::type >;

		template< typename t >
			struct push< t, typename enable_if< is_stdcontainer< t > >::type >;
	}

	template< typename t >
		struct lua::to< t, typename enable_if< is_stdcontainer< t > >::type >
	{
		static void call( stack& stack, typename call_traits< t >::reference container, s32 index )
		{
			type_check< t >( stack, index, type::table );

			typedef typename no_ref< t >::type type;
			up_t table_size = stack.objlen( index );

			type out;
			reserve( out, table_size );
			std::insert_iterator< type > j( out, out.begin() );

			for ( up_t i = 0; i != table_size; ++i, ++j )
			{
				stack.raw_geti( index, i + 1 );

				typename type::value_type element;
				to< typename type::value_type >::call( stack, element, -1 );
				*j = element;

				stack.pop( 1 );
			}

			container.swap( out );
		}
	};

	template< typename t >
		struct lua::push< t, typename enable_if< is_stdcontainer< t > >::type >
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
		to< typename tuple_element< n, t >::type >::call( stack, tuple._ ## n, -1 );\
		stack.pop( 1 );

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
			type_check< t >( stack, index, type::table );
			up_t table_size = stack.objlen( index );

			if ( table_size != LIMIT )
				throw error::lua() <<
					"Table is of size " << table_size << ", tuple is of size " << LIMIT;

			BOOST_PP_REPEAT( LIMIT, TUPLE_TO, ~ )
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
