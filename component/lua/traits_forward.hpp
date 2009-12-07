/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_LUA_TRAITS_FORWARD_HPP
#define OOE_COMPONENT_LUA_TRAITS_FORWARD_HPP

#include <cstring>

#include "component/lua/error.hpp"
#include "foundation/utility/hint.hpp"

namespace ooe
{
	namespace lua
	{
		template< typename >
			struct is_arithmetic;

		template< typename >
			struct is_pointer;

		template< typename >
			struct is_pod;

		template< typename >
			struct is_class;

//--- lua::to ------------------------------------------------------------------
		template< typename, typename = void >
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

		template< typename t >
			struct to< t, typename enable_if< is_pod< t > >::type >;

		template< typename t >
			struct to< t, typename enable_if< is_class< t > >::type >;

		template< typename t >
			struct to< t, typename enable_if< is_construct< t > >::type >;

		template< typename t >
			struct to< t, typename enable_if< is_destruct< t > >::type >;

		template< typename t >
			struct to< t, typename enable_if< is_array< t > >::type >;

//--- lua::push ----------------------------------------------------------------
		template< typename, typename = void >
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

		template< typename t >
			struct push< t, typename enable_if< is_pod< t > >::type >;

		template< typename t >
			struct push< t, typename enable_if< is_class< t > >::type >;

		template< typename t >
			struct push< t, typename enable_if< is_construct< t > >::type >;

		template< typename t >
			struct push< t, typename enable_if< is_destruct< t > >::type >;

		template< typename t >
			struct push< t, typename enable_if< is_array< t > >::type >;

//--- type_check ---------------------------------------------------------------
		inline void type_check( stack&, s32, const std::type_info&, type::id );

//--- meta_set -----------------------------------------------------------------
		inline void meta_set( stack&, s32, const std::type_info&, lua::cfunction );

//--- destruct -----------------------------------------------------------------
		template< typename >
			s32 destruct( state* );

//--- destroy ------------------------------------------------------------------
		template< typename >
			s32 destroy( state* );
	}

//--- lua::is_arithmetic -------------------------------------------------------
	template< typename t >
		struct lua::is_arithmetic
	{
		static const bool value =
			!is_boolean< t >::value &&
			( ooe::is_arithmetic< typename no_ref< t >::type >::value ||
			is_enum< typename no_ref< t >::type >::value );
	};

//--- lua::is_pointer ----------------------------------------------------------
	template< typename t >
		struct lua::is_pointer
	{
		static const bool value =
			!is_cstring< t >::value &&
			ooe::is_pointer< typename no_ref< t >::type >::value;
	};

//--- lua::is_pod --------------------------------------------------------------
	template< typename t >
		struct lua::is_pod
	{
		static const bool value =
			!is_boolean< t >::value &&
			!is_arithmetic< t >::value &&
			!is_cstring< t >::value &&
			!is_pointer< t >::value &&
			( ooe::is_pod< typename no_ref< t >::type >::value ||
			has_trivial_copy< typename no_ref< t >::type >::value );
	};

//--- lua::is_class ------------------------------------------------------------
	template< typename t >
		struct lua::is_class
	{
		static const bool value =
			!is_construct< t >::value &&
			!is_destruct< t >::value &&
			!is_stdstring< t >::value &&
			!is_stdcontainer< t >::value &&
			!is_tuple< t >::value &&
			!has_trivial_copy< typename no_ref< t >::type >::value &&
			( ooe::is_class< typename no_ref< t >::type >::value ||
			is_union< typename no_ref< t >::type >::value );
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
		static void call( stack& stack, typename call_traits< t >::reference, s32 index )
		{
			typedef typename no_ref< t >::type type;
			type_check( stack, index, typeid( type ), lua::type::nil );
		}
	};

	template< typename t >
		struct lua::push< t, typename enable_if< is_empty< t > >::type >
	{
		static void call( stack& stack, typename call_traits< t >::param_type )
		{
			stack.push_nil();
		}
	};

//--- lua::traits: boolean -----------------------------------------------------
	template< typename t >
		struct lua::to< t, typename enable_if< is_boolean< t > >::type >
	{
		static void call( stack& stack, bool& boolean, s32 index )
		{
			typedef typename no_ref< t >::type type;
			type_check( stack, index, typeid( type ), lua::type::boolean );
			boolean = stack.to_boolean( index );
		}
	};

	template< typename t >
		struct lua::push< t, typename enable_if< is_boolean< t > >::type >
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
		static void call( stack& stack, typename call_traits< t >::reference number, s32 index )
		{
			typedef typename no_ref< t >::type type;
			type_check( stack, index, typeid( type ), lua::type::number );
			number = stack.to_number( index );
		}
	};

	template< typename t >
		struct lua::push< t, typename enable_if< lua::is_arithmetic< t > >::type >
	{
		static void call( stack& stack, typename call_traits< t >::param_type number )
		{
			stack.push_number( number );
		}
	};

//--- lua::traits: pointer -----------------------------------------------------
	template< typename t >
		struct lua::to< t, typename enable_if< lua::is_pointer< t > >::type >
	{
		static void call( stack& stack, typename call_traits< t >::reference pointer, s32 index )
		{
			typedef typename no_ref< t >::type type;
			type_check( stack, index, typeid( type ), lua::type::userdata );
			pointer = *static_cast< type* >( stack.to_userdata( index ) );
		}
	};

	template< typename t >
		struct lua::push< t, typename enable_if< lua::is_pointer< t > >::type >
	{
		static void call( stack& stack, typename call_traits< t >::param_type pointer )
		{
			typedef typename no_ref< t >::type type;
			new( stack.new_userdata( sizeof( type ) ) ) type( pointer );
		}
	};

//--- lua::traits: string ------------------------------------------------------
	template< typename t >
		struct lua::to< t, typename enable_if< is_string< t > >::type >
	{
		static void call( stack& stack, typename call_traits< t >::reference string, s32 index )
		{
			typedef typename no_ref< t >::type type;
			type_check( stack, index, typeid( type ), lua::type::string );

			up_t size;
			const c8* data = stack.to_lstring( index, &size );
			string = string_make< type >( data, size );
		}
	};

	template< typename t >
		struct lua::push< t, typename enable_if< is_string< t > >::type >
	{
		static void call( stack& stack, typename call_traits< t >::param_type string )
		{
			stack.push_lstring( string_data( string ), string_size( string ) );
		}
	};

//--- lua::traits: pod ---------------------------------------------------------
	template< typename t >
		struct lua::to< t, typename enable_if< lua::is_pod< t > >::type >
	{
		static void call( stack& stack, typename call_traits< t >::reference pod, s32 index )
		{
			typedef typename no_ref< t >::type type;
			type_check( stack, index, typeid( type ), lua::type::userdata );
			std::memcpy( &pod, stack.to_userdata( index ), sizeof( type ) );
		}
	};

	template< typename t >
		struct lua::push< t, typename enable_if< lua::is_pod< t > >::type >
	{
		static void call( stack& stack, typename call_traits< t >::param_type pod )
		{
			typedef typename no_ref< t >::type type;
			std::memcpy( stack.new_userdata( sizeof( type ) ), &pod, sizeof( type ) );
		}
	};

//--- lua::traits: class -------------------------------------------------------
	template< typename t >
		struct lua::to< t, typename enable_if< lua::is_class< t > >::type >
	{
		static void call( stack& stack, typename call_traits< t >::reference class_, s32 index )
		{
			typedef typename no_ref< t >::type type;
			type_check( stack, index, typeid( type ), lua::type::userdata );
			class_ = *static_cast< type* >( stack.to_userdata( index ) );
		}
	};

	template< typename t >
		struct lua::push< t, typename enable_if< lua::is_class< t > >::type >
	{
		static void call( stack& stack, typename call_traits< t >::param_type class_ )
		{
			typedef typename no_ref< t >::type type;
			new( stack.new_userdata( sizeof( type ) ) ) type( class_ );
			meta_set( stack, -2, typeid( type ), destruct< type > );
		}
	};

//--- lua::traits: construct -------------------------------------------------------
	template< typename INVALID_USAGE >
		struct lua::to< INVALID_USAGE, typename enable_if< is_construct< INVALID_USAGE > >::type >
	{
		static void call( stack&, INVALID_USAGE, s32 ) OOE_CONST
		{
			BOOST_STATIC_ASSERT( !sizeof( INVALID_USAGE ) );
		}
	};

	template< typename t >
		struct lua::push< t, typename enable_if< is_construct< t > >::type >
	{
		static void call( stack& stack, typename call_traits< t >::param_type construct )
		{
			typedef typename no_ref< t >::type type;
			typedef typename type::pointer pointer;
			new( stack.new_userdata( sizeof( pointer ) ) ) pointer( construct );
			meta_set( stack, -2, typeid( type ), destroy< typename type::value_type > );
		}
	};

//--- lua::traits: destruct ----------------------------------------------------
	template< typename t >
		struct lua::to< t, typename enable_if< is_destruct< t > >::type >
	{
		static void call( stack& stack, typename call_traits< t >::reference destruct, s32 index )
		{
			typedef typename no_ref< t >::type type;
			type_check( stack, index, typeid( type ), lua::type::userdata );

			destruct = *static_cast< typename type::pointer* >( stack.to_userdata( index ) );
			stack.push_nil();
			stack.set_metatable( index );
		}
	};

	template< typename INVALID_USAGE >
		struct lua::push< INVALID_USAGE, typename enable_if< is_destruct< INVALID_USAGE > >::type >
	{
		static void call( stack&, INVALID_USAGE ) OOE_CONST
		{
			BOOST_STATIC_ASSERT( !sizeof( INVALID_USAGE ) );
		}
	};

//--- lua::traits: array -------------------------------------------------------
	template< typename t >
		struct lua::to< t, typename enable_if< is_array< t > >::type >
	{
		static void call( stack& stack, typename call_traits< t >::reference array, s32 index )
		{
			typedef typename no_ref< t >::type type;
			type_check( stack, index, typeid( type ), lua::type::table );

			up_t table_size = stack.objlen( index );
			up_t array_size = extent< type >::value;

			if ( table_size != array_size )
				throw error::lua() <<
					"Table is of size " << table_size << ", array is of size " << array_size;

			for ( up_t i = 0; i != array_size; ++i )
			{
				stack.raw_geti( index, i + 1 );
				to< typename remove_extent< type >::type >::call( stack, array[ i ], -1 );
				stack.pop( 1 );
			}
		}
	};

	template< typename t >
		struct lua::push< t, typename enable_if< is_array< t > >::type >
	{
		static void call( stack& stack, typename call_traits< t >::param_type array )
		{
			typedef typename no_ref< t >::type type;
			up_t array_size = extent< type >::value;
			stack.create_table( array_size );

			for ( up_t i = 0; i != array_size; ++i )
			{
				push< typename remove_extent< type >::type >::call( stack, array[ i ] );
				stack.raw_seti( -2, i + 1 );
			}
		}
	};

//--- lua::type_check ----------------------------------------------------------
	inline void lua::type_check( stack& stack, s32 index, const std::type_info& type_info,
		type::id id )
	{
		type::id type = stack.type( index );

		if ( type == id )
			return;

		stack.where();
		throw error::lua( stack ) << "bad argument at index " << index << " for type \"" <<
			demangle( type_info.name() ) << "\" (" << stack.type_name( id ) << " expected, got " <<
			stack.type_name( type ) << ')';
	}

//--- lua::meta_set ------------------------------------------------------------
	inline void lua::meta_set( stack& stack, s32 index, const std::type_info& type_info,
		lua::cfunction gc )
	{
		if ( stack.new_metatable( type_info.name() ) )
		{
			push< const c8* >::call( stack, "__gc" );
			stack.push_cclosure( gc );
			stack.raw_set( -3 );
		}

		stack.set_metatable( index );
	}

//--- lua::destruct ------------------------------------------------------------
	template< typename t >
		s32 lua::destruct( state* state )
	{
		stack stack( state );
		static_cast< t* >( stack.to_userdata( 1 ) )->~t();
		return 0;
	}

//--- lua::destroy -------------------------------------------------------------
	template< typename t >
		s32 lua::destroy( state* state )
	{
		stack stack( state );
		delete *static_cast< t** >( stack.to_userdata( 1 ) );
		return 0;
	}
}

#endif	// OOE_COMPONENT_LUA_TRAITS_FORWARD_HPP
