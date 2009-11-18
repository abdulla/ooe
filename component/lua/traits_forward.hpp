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

//--- type_check ---------------------------------------------------------------
		template< typename >
			void type_check( stack&, s32, type::id );
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
		static void call( stack&, NO_SPECIALISATION_DEFINED, s32 ) OOE_CONST
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

//--- lua::traits: empty -------------------------------------------------------
	template< typename t >
		struct lua::to< t, typename enable_if< is_empty< t > >::type >
	{
		static void call( stack& stack, call_traits< t >::reference, s32 index )
		{
			type_check< typename no_ref< t >::type >( stack, index, type::nil );
		}
	};

	template< typename t >
		struct lua::push< t, typename enable_if< is_empty< t > >::type >
	{
		static void call( stack& stack, call_traits< t >::param_type )
		{
			stack.push_nil();
		}
	};

//--- lua::traits: boolean -----------------------------------------------------
	template< typename t >
		struct lua::to< t, typename enable_if< lua::is_boolean< t > >::type >
	{
		static void call( stack& stack, bool& boolean, s32 index )
		{
			type_check< bool >( stack, index, type::boolean );
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

//--- lua::traits: arithmetic --------------------------------------------------
	template< typename t >
		struct lua::to< t, typename enable_if< lua::is_arithmetic< t > >::type >
	{
		static void call( stack& stack, call_traits< t >::reference number, s32 index )
		{
			type_check< typename no_ref< t >::type >( stack, index, type::number );
			number = stack.to_number( index );
		}
	};

	template< typename t >
		struct lua::push< t, typename enable_if< lua::is_arithmetic< t > >::type >
	{
		static void call( stack& stack, call_traits< t >::param_type number )
		{
			stack.push_number( number );
		}
	};

//--- lua::traits: pointer -----------------------------------------------------
	template< typename t >
		struct lua::to< t, typename enable_if< lua::is_pointer< t > >::type >
	{
		static void call( stack& stack, call_traits< t >::reference pointer, s32 index )
		{
			type_check< typename no_ref< t >::type >( stack, index, type::number );
			pointer = stack.to_pointer( index );
		}
	};

	template< typename t >
		struct lua::push< t, typename enable_if< lua::is_pointer< t > >::type >
	{
		static void call( stack& stack, call_traits< t >::param_type pointer )
		{
			stack.push_lightuserdata( const_cast< typename no_qual< t >::type* >( pointer ) );
		}
	};

//--- lua::traits: string ------------------------------------------------------
	template< typename t >
		struct lua::to< t, typename enable_if< is_string< t > >::type >
	{
		static void call( stack& stack, call_traits< t >::reference string, s32 index )
		{
			typedef typename no_ref< t >::type type;
			type_check< type >( stack, index, type::string );

			up_t size;
			const c8* data = stack.to_lstring( index, &size );
			string = string_make< type >( data, size );
		}
	};

	template< typename t >
		struct lua::push< t, typename enable_if< is_string< t > >::type >
	{
		static void call( stack& stack, call_traits< t >::param_type string, s32 index )
		{
			stack.push_lstring( string_data( string ), string_size( string ) );
		}
	};

//--- lua::type_check ----------------------------------------------------------
	template< typename t >
		void lua::type_check( stack& stack, s32 index, type::id id )
	{
		type::id type = stack.type( index );

		if ( type == id )
			return;

		const c8* name = stack.type_name( type );
		stack.where();
		throw error::lua( stack ) << "bad argument at index " << index <<
			" (" << demangle( typeid( t ).name() ) << " expected, got " << name << " )";
	}
}

#endif	// OOE_COMPONENT_LUA_TRAITS_HPP
