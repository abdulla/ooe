/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_LUA_TRAITS_HPP
#define OOE_COMPONENT_LUA_TRAITS_HPP

#include "foundation/utility/macro.hpp"
#include "foundation/utility/string.hpp"
#include "foundation/utility/traits.hpp"

namespace ooe
{
	namespace lua
	{
		template< typename >
			struct is_boolean;

		template< typename >
			struct is_arithmetic;

		template< typename >
			struct is_pointer;

//--- lua::to ------------------------------------------------------------------
		template< typename, typename >
			struct to;

		template< typename t >
			struct to< t, typename enable_if< is_empty< t > >::type >;

		template< typename t >
			struct to< t, typename enable_if< is_boolean< t > >::type >;

//--- lua::push ----------------------------------------------------------------
		template< typename, typename >
			struct push;

		template< typename t >
			struct push< t, typename enable_if< is_empty< t > >::type >;

		template< typename t >
			struct push< t, typename enable_if< is_boolean< t > >::type >;
	}

//--- lua::traits: default -----------------------------------------------------
	template< typename NO_SPECIALISATION_DEFINED, typename >
		struct lua::to
	{
		static NO_SPECIALISATION_DEFINED call( const stack&, s32 ) OOE_CONST
		{
			BOOST_STATIC_ASSERT( !sizeof( NO_SPECIALISATION_DEFINED ) );
			return NO_SPECIALISATION_DEFINED();
		}
	};

	template< typename NO_SPECIALISATION_DEFINED, typename >
		struct lua::push
	{
		static void call( stack&, NO_SPECIALISATION_DEFINED ) OOE_CONST
		{
			BOOST_STATIC_ASSERT( !sizeof( NO_SPECIALISATION_DEFINED ) );
		}
	};
}

#endif	// OOE_COMPONENT_LUA_TRAITS_HPP
