/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_LUA_TRAITS_FORWARD_HPP
#define OOE_COMPONENT_LUA_TRAITS_FORWARD_HPP

#include "component/lua/error.hpp"
#include "component/registry/traits.hpp"
#include "foundation/utility/hint.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( lua ) )

inline void meta_set( stack&, s32, const std::type_info&, cfunction = 0 );
inline void type_check( stack&, s32, type::id );
inline void type_check( stack&, s32, const std::type_info& );

//--- destroy --------------------------------------------------------------------------------------
template< typename t >
	s32 destroy( state* state )
{
	stack stack( state );
	delete *static_cast< t** >( stack.to_userdata( 1 ) );
	return 0;
}

//--- is_arithmetic --------------------------------------------------------------------------------
template< typename t >
	struct is_arithmetic
{
	static const bool value =
		!component::is_boolean< t >::value &&
		( ooe::is_arithmetic< typename no_ref< t >::type >::value ||
		is_enum< typename no_ref< t >::type >::value );
};

//--- traits: default ------------------------------------------------------------------------------
template< typename NO_SPECIALISATION_DEFINED, typename = void >
	struct to
{
	static void call( stack&, NO_SPECIALISATION_DEFINED, s32 ) OOE_CONST
	{
		OOE_STATIC_ASSERT( !sizeof( NO_SPECIALISATION_DEFINED ) );
	}
};

template< typename NO_SPECIALISATION_DEFINED, typename = void >
	struct push
{
	static void call( stack&, NO_SPECIALISATION_DEFINED ) OOE_CONST
	{
		OOE_STATIC_ASSERT( !sizeof( NO_SPECIALISATION_DEFINED ) );
	}
};

//--- traits: empty --------------------------------------------------------------------------------
template< typename t >
	struct to< t, typename enable_if< is_empty< t > >::type >
{
	static void call( stack& stack, typename call_traits< t >::reference, s32 index )
	{
		typedef typename no_ref< t >::type type;
		type_check( stack, index, lua::type::nil );
	}
};

template< typename t >
	struct push< t, typename enable_if< is_empty< t > >::type >
{
	static void call( stack& stack, typename call_traits< t >::param_type )
	{
		stack.push_nil();
	}
};

//--- traits: boolean ------------------------------------------------------------------------------
template< typename t >
	struct to< t, typename enable_if< component::is_boolean< t > >::type >
{
	static void call( stack& stack, bool& boolean, s32 index )
	{
		typedef typename no_ref< t >::type type;
		type_check( stack, index, lua::type::boolean );
		boolean = stack.to_boolean( index );
	}
};

template< typename t >
	struct push< t, typename enable_if< component::is_boolean< t > >::type >
{
	static void call( stack& stack, bool boolean )
	{
		stack.push_boolean( boolean );
	}
};

//--- traits: arithmetic ---------------------------------------------------------------------------
template< typename t >
	struct to< t, typename enable_if< is_arithmetic< t > >::type >
{
	static void call( stack& stack, typename call_traits< t >::reference number, s32 index )
	{
		typedef typename no_ref< t >::type type;
		type_check( stack, index, lua::type::number );
		number = stack.to_number( index );
	}
};

template< typename t >
	struct push< t, typename enable_if< is_arithmetic< t > >::type >
{
	static void call( stack& stack, typename call_traits< t >::param_type number )
	{
		stack.push_number( number );
	}
};

//--- traits: string -------------------------------------------------------------------------------
template< typename t >
	struct to< t, typename enable_if< is_string< t > >::type >
{
	static void call( stack& stack, typename call_traits< t >::reference string, s32 index )
	{
		typedef typename no_ref< t >::type type;
		type_check( stack, index, lua::type::string );

		up_t size;
		const c8* data = stack.to_lstring( index, &size );
		string = string_make< type >( data, size );
	}
};

template< typename t >
	struct push< t, typename enable_if< is_string< t > >::type >
{
	static void call( stack& stack, typename call_traits< t >::param_type string )
	{
		stack.push_lstring( string_data( string ), string_size( string ) );
	}
};

//--- traits: pointer ------------------------------------------------------------------------------
template< typename t >
	struct to< t, typename enable_if< component::is_pointer< t > >::type >
{
	static void call( stack& stack, typename call_traits< t >::reference pointer, s32 index )
	{
		type_check( stack, index, typeid( typename no_qual< t >::type ) );
		pointer = *static_cast< typename no_ref< t >::type* >( stack.to_userdata( index ) );
	}
};

template< typename t >
	struct push< t, typename enable_if< component::is_pointer< t > >::type >
{
	static void call( stack& stack, typename call_traits< t >::param_type pointer )
	{
		new( stack.new_userdata( sizeof( void* ) ) ) typename no_ref< t >::type( pointer );
		meta_set( stack, -1, typeid( typename no_qual< t >::type ) );
	}
};

//--- traits: class --------------------------------------------------------------------------------
template< typename t >
	struct to< t, typename enable_if< component::is_class< t > >::type >
{
	static void call( stack& stack, typename call_traits< t >::reference class_, s32 index )
	{
		typedef typename no_ref< t >::type type;
		type_check( stack, index, typeid( type ) );
		class_ = **static_cast< type** >( stack.to_userdata( index ) );
	}
};

template< typename t >
	struct push< t, typename enable_if< component::is_class< t > >::type >
{
	static void call( stack& stack, typename call_traits< t >::param_type class_ )
	{
		typedef typename no_ref< t >::type type;
		new( stack.new_userdata( sizeof( void* ) ) ) type*( new type( class_ ) );
		meta_set( stack, -1, typeid( type ), destroy< type > );
	}
};

//--- traits: construct ----------------------------------------------------------------------------
template< typename INVALID_USAGE >
	struct to< INVALID_USAGE, typename enable_if< is_construct< INVALID_USAGE > >::type >
{
	static void call( stack&, INVALID_USAGE, s32 ) OOE_CONST
	{
		OOE_STATIC_ASSERT( !sizeof( INVALID_USAGE ) );
	}
};

template< typename t >
	struct push< t, typename enable_if< is_construct< t > >::type >
{
	static void call( stack& stack, typename call_traits< t >::param_type construct )
	{
		typedef typename no_ref< t >::type::value_type type;
		new( stack.new_userdata( sizeof( void* ) ) ) type*( construct );
		meta_set( stack, -1, typeid( type ), destroy< type > );
	}
};

//--- traits: destruct -----------------------------------------------------------------------------
template< typename t >
	struct to< t, typename enable_if< is_destruct< t > >::type >
{
	static void call( stack& stack, typename call_traits< t >::reference destruct, s32 index )
	{
		typedef typename no_ref< t >::type::value_type type;
		type_check( stack, index, typeid( type ) );

		destruct = *static_cast< type** >( stack.to_userdata( index ) );
		stack.push_nil();
		stack.set_metatable( index );
	}
};

template< typename INVALID_USAGE >
	struct push< INVALID_USAGE, typename enable_if< is_destruct< INVALID_USAGE > >::type >
{
	static void call( stack&, INVALID_USAGE ) OOE_CONST
	{
		OOE_STATIC_ASSERT( !sizeof( INVALID_USAGE ) );
	}
};

//--- traits: array --------------------------------------------------------------------------------
template< typename t >
	struct to< t, typename enable_if< is_array< t > >::type >
{
	static void call( stack& stack, typename call_traits< t >::reference array, s32 index )
	{
		typedef typename no_ref< t >::type type;
		type_check( stack, index, lua::type::table );

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
	struct push< t, typename enable_if< is_array< t > >::type >
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

//--- meta_set -------------------------------------------------------------------------------------
inline void meta_set( stack& stack, s32 index, const std::type_info& type_info, cfunction gc )
{
	stack.create_table( 1, gc ? 1 : 0 );

	stack.push_lightuserdata( &type_info );
	stack.raw_seti( -2, 1 );

	if ( gc )
	{
		push< const c8* >::call( stack, "__gc" );
		stack.push_cclosure( gc );
		stack.raw_set( -3 );
	}

	stack.set_metatable( index - 1 );
}

//--- type_check -----------------------------------------------------------------------------------
inline void type_check( stack& stack, s32 index, type::id id )
{
	type::id type = stack.type( index );

	if ( type == id )
		return;

	stack.where();
	throw error::lua( stack ) << "bad argument at index " << index << " (" <<
		stack.type_name( id ) << " expected, got " << stack.type_name( type ) << ')';
}

inline void type_check( stack& stack, s32 index, const std::type_info& type_x )
{
	type_check( stack, index, lua::type::userdata );
	stack.get_metatable( index );
	type_check( stack, -1, lua::type::table );
	stack.raw_geti( -1, 1 );
	type_check( stack, -1, lua::type::lightuserdata );

	const std::type_info& type_y = *static_cast< std::type_info* >( stack.to_userdata( -1 ) );
	stack.pop( 2 );

	if ( type_x == type_y )
		return;

	stack.where();
	throw error::lua( stack ) << "Types do not match, \"" << demangle( type_x.name() ) <<
		"\" != \"" << demangle( type_y.name() ) << '\"';
}

OOE_NAMESPACE_END( ( ooe )( lua ) )

#endif	// OOE_COMPONENT_LUA_TRAITS_FORWARD_HPP
