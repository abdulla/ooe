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

		template< typename t >
			struct to< t, typename enable_if< is_arithmetic< t > >::type >;

		template< typename t >
			struct to< t, typename enable_if< is_pointer< t > >::type >;

		template< typename t >
			struct to< t, typename enable_if< is_string< t > >::type >;

//--- lua::push ----------------------------------------------------------------
		template< typename, typename >
			struct push;

		template< typename t >
			struct push< t, typename enable_if< is_empty< t > >::type >;

		template< typename t >
			struct push< t, typename enable_if< is_boolean< t > >::type >;

		template< typename t >
			struct push< t, typename enable_if< is_arithmetic< t > >::type >;

		template< typename t >
			struct push< t, typename enable_if< is_pointer< t > >::type >;

		template< typename t >
			struct push< t, typename enable_if< is_string< t > >::type >;
	}

//--- lua::is_boolean ----------------------------------------------------------
	template< typename t >
		struct lua::is_boolean
		: public is_like< t, bool >
	{
	};

//--- lua::is_arithmetic -------------------------------------------------------
	template< typename t >
		struct lua::is_arithmetic
	{
		static const bool value = // !is_boolean< t >::value &&
			( ooe::is_arithmetic< typename no_ref< t >::type >::value ||
			is_enum< typename no_ref< t >::type >::value );
	};

//--- lua::is_pointer ----------------------------------------------------------
	template< typename t >
		struct lua::is_pointer
	{
		static const bool value = !is_cstring< t >::value &&
			ooe::is_pointer< typename no_ref< t >::type >::value;
	};

//--- lua::traits: default -----------------------------------------------------
	template< typename NO_SPECIALISATION_DEFINED, typename >
		struct lua::to
	{
		static void call( const stack&, NO_SPECIALISATION_DEFINED, s32 ) OOE_CONST
		{
			BOOST_STATIC_ASSERT( !sizeof( NO_SPECIALISATION_DEFINED ) );
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

//--- lua::traits: boolean -----------------------------------------------------
	template< typename t >
		struct lua::to< t, typename enable_if< lua::is_boolean< t > >::type >
	{
		static void call( const stack& stack, bool& boolean, s32 index )
		{
			// do type check
			boolean = stack.to_boolean( index );
		}
	};

	template< typename t >
		struct lua::push< t, typename enable_if< lua::is_boolean< t > >::type >
	{
		static void call( stack& stack, bool boolean )
		{
			stack.push_boolean( boolean );
		}
	};
}

#endif	// OOE_COMPONENT_LUA_TRAITS_HPP
