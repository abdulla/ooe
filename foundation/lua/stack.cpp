/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

extern "C"
{
	#include <lauxlib.h>
}

#include "foundation/lua/error.hpp"

namespace ooe
{
//--- lua::stack ---------------------------------------------------------------
	lua::stack::stack( lua::state* state_ )
		: state( state_ )
	{
	}

	u32 lua::stack::gettop( void )
	{
		return lua_gettop( state );
	}

	void lua::stack::pop( u32 count )
	{
		return lua_settop( state, -1 - count );
	}

	void lua::stack::pushvalue( s32 index )
	{
		lua_pushvalue( state, index );
	}

	void lua::stack::remove( s32 index )
	{
		lua_remove( state, index );
	}

	void lua::stack::insert( s32 index )
	{
		lua_insert( state, index );
	}

	void lua::stack::replace( s32 index )
	{
		lua_replace( state, index );
	}

	lua::id::type lua::stack::type( s32 index ) const
	{
		return static_cast< id::type >( lua_type( state, index ) );
	}

	const c8* lua::stack::type_name( id::type id ) const
	{
		return lua_typename( state, id );
	}

	f32 lua::stack::tonumber( s32 index ) const
	{
		return static_cast< f32 >( lua_tonumber( state, index ) );
	}

	bool lua::stack::toboolean( s32 index ) const
	{
		return lua_toboolean( state, index );
	}

	const c8* lua::stack::tolstring( s32 index, up_t* length ) const
	{
		return lua_tolstring( state, index, length );
	}

	up_t lua::stack::objlen( s32 index ) const
	{
		return lua_objlen( state, index );
	}

	void* lua::stack::touserdata( s32 index ) const
	{
		return lua_touserdata( state, index );
	}

	void lua::stack::pushnil( void )
	{
		lua_pushnil( state );
	}

	void lua::stack::pushnumber( f32 number )
	{
		lua_pushnumber( state, number );
	}

	void lua::stack::pushlstring( const c8* string, up_t length )
	{
		lua_pushlstring( state, string, length );
	}

	void lua::stack::pushcclosure( cfunction function, u32 upvalues )
	{
		lua_pushcclosure( state, function, upvalues );
	}

	void lua::stack::pushboolean( bool boolean )
	{
		lua_pushboolean( state, boolean );
	}

	void lua::stack::pushlightuserdata( void* pointer )
	{
		lua_pushlightuserdata( state, pointer );
	}

	void lua::stack::rawget( s32 index )
	{
		lua_rawget( state, index );
	}

	void lua::stack::rawgeti( s32 index, u32 element )
	{
		lua_rawgeti( state, index, element );
	}

	void lua::stack::createtable( u32 array_elements, u32 table_elements )
	{
		lua_createtable( state, array_elements, table_elements );
	}

	void* lua::stack::newuserdata( up_t size )
	{
		return lua_newuserdata( state, size );
	}

	bool lua::stack::getmetatable( s32 index )
	{
		return lua_getmetatable( state, index );
	}

	void lua::stack::rawset( s32 index )
	{
		lua_rawset( state, index );
	}

	void lua::stack::rawseti( s32 index, u32 element )
	{
		lua_rawseti( state, index, element );
	}

	void lua::stack::setmetatable( s32 index )
	{
		lua_setmetatable( state, index );
	}

	void lua::stack::call( u32 args, u32 results )
	{
		lua_call( state, args, results );
	}

	void lua::stack::pcall( u32 args, u32 results )
	{
		if ( lua_pcall( state, args, results, 0 ) )
			throw error::lua( *this );
	}

	void lua::stack::error( void )
	{
		lua_error( state );
	}

	bool lua::stack::next( s32 index )
	{
		return lua_next( state, index );
	}

	void lua::stack::concat( u32 count )
	{
		lua_concat( state, count );
	}

	bool lua::stack::newmetatable( const c8* name )
	{
		return luaL_newmetatable( state, name );
	}

	void lua::stack::getmetatable( const c8* name )
	{
		lua_getfield( state, registry, name );
	}

	void lua::stack::where( u32 level )
	{
		luaL_where( state, level );
	}

	s32 lua::stack::ref( void )
	{
		return luaL_ref( state, registry );
	}

	void lua::stack::unref( s32 reference )
	{
		luaL_unref( state, registry, reference );
	}
}
