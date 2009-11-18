/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_LUA_TRAITS_HPP
#define OOE_COMPONENT_LUA_TRAITS_HPP

#include "component/lua/traits_forward.hpp"

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
		static void call( stack& stack, call_traits< t >::reference container, s32 index )
		{
			typedef typename no_ref< t >::type type;
			type_check< type >( stack, index, type::table );
			up_t table_size = stack.objlen( index );
			type out;
			reserve( out, table_size );

			for ( up_t i = 0; i != table_size; ++i )
			{
				stack.raw_geti( index, i + 1 );
				typename type::value_type value;
				to< typename type::value_type >::call( stack, value, -1 );
				out.insert( out.end(), value );
				stack.pop( 1 );
			}

			container.swap( out );
		}
	};

	template< typename t >
		struct lua::push< t, typename enable_if< is_stdcontainer< t > >::type >
	{
		static void call( stack& stack, call_traits< t >::param_type container )
		{
		}
	};
}

#endif	// OOE_COMPONENT_LUA_TRAITS_HPP
