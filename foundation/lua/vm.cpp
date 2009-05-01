/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

extern "C"
{
	#include <lauxlib.h>
	#include <lualib.h>
}

#include "foundation/io/memory.hpp"
#include "foundation/lua/error.hpp"
#include "foundation/lua/traits.hpp"
#include "foundation/lua/vm.hpp"

namespace ooe
{
//--- lua::vm ------------------------------------------------------------------
	lua::vm::vm( void )
		: state( luaL_newstate() )
	{
		lua_cpcall( state, luaopen_base, 0 );
		lua_cpcall( state, luaopen_table, 0 );
		lua_cpcall( state, luaopen_string, 0 );
		lua_cpcall( state, luaopen_math, 0 );
	}

	lua::vm::~vm( void )
	{
		lua_close( state );
	}

	void lua::vm::load( const std::string& name, const descriptor& desc )
	{
		memory memory( desc );

		if ( luaL_loadbuffer( state, memory.as< c8 >(), memory.size(), name.c_str() ) ||
			lua_pcall( state, 0, 0, 0 ) )
		{
			lua::stack stack_( this->stack() );
			throw error::lua( stack_ );
		}
	}

	void lua::vm::collect( void )
	{
		lua_gc( state, LUA_GCCOLLECT, 0 );
	}

	u32 lua::vm::size( void ) const
	{
		return lua_gc( state, LUA_GCCOUNT, 0 ) * 1024 + lua_gc( state, LUA_GCCOUNTB, 0 );
	}

	lua::stack lua::vm::stack( void ) const
	{
		return lua::stack( state );
	}

	std::string lua::vm::version( void ) const
	{
		lua::stack s( stack() );
		traits< const c8* >::push( s, "_VERSION" );
		s.rawget( global );
		std::string v = traits< std::string >::to( s, -1 );
		s.pop( 1 );
		return v;
	}
}
