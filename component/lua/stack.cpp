/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

extern "C"
{
	#include <lauxlib.h>
}

#include "component/lua/error.hpp"

namespace ooe
{
//--- lua::stack ---------------------------------------------------------------
	lua::stack::stack( lua::state* state_ )
		: state( state_ )
	{
	}

	void lua::stack::pop( u32 size )
	{
		lua_settop( state, -1 - size );
	}

	lua::type::id lua::stack::type( s32 index ) const
	{
		return static_cast< type::id >( lua_type( state, index ) );
	}

	const c8* lua::stack::type_name( type::id id ) const
	{
		return lua_typename( state, id );
	}

	f64 lua::stack::to_number( s32 index ) const
	{
		return lua_tonumber( state, index );
	}

	bool lua::stack::to_boolean( s32 index ) const
	{
		return lua_toboolean( state, index );
	}

	const c8* lua::stack::to_lstring( s32 index, up_t* size ) const
	{
		return lua_tolstring( state, index, size );
	}

	void* lua::stack::to_userdata( s32 index ) const
	{
		return lua_touserdata( state, index );
	}

	void lua::stack::push_nil( void )
	{
		lua_pushnil( state );
	}

	void lua::stack::push_number( f64 number )
	{
		lua_pushnumber( state, number );
	}

	void lua::stack::push_lstring( const c8* string, up_t size )
	{
		lua_pushlstring( state, string, size );
	}

	void lua::stack::push_cclosure( cfunction function, u32 upvalues )
	{
		lua_pushcclosure( state, function, upvalues );
	}

	void lua::stack::push_boolean( bool boolean )
	{
		lua_pushboolean( state, boolean );
	}

	void lua::stack::push_lightuserdata( void* pointer )
	{
		lua_pushlightuserdata( state, pointer );
	}

	void lua::stack::raw_get( s32 index )
	{
		lua_rawget( state, index );
	}

	void lua::stack::raw_geti( s32 index, u32 array_index )
	{
		lua_rawgeti( state, index, array_index );
	}

	void lua::stack::create_table( u32 array_size, u32 table_size )
	{
		lua_createtable( state, array_size, table_size );
	}

	void lua::stack::raw_set( s32 index )
	{
		lua_rawset( state, index );
	}

	void lua::stack::raw_seti( s32 index, u32 array_index )
	{
		lua_rawseti( state, index, array_index );
	}
}
