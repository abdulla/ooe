/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

extern "C"
{
	#include <lauxlib.h>
	#include <lualib.h>
}

#include "component/lua/error.hpp"
#include "component/lua/vm.hpp"
#include "foundation/io/memory.hpp"

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
			lua::stack stack_( state );
			throw error::lua( stack_ );
		}
	}

	void lua::vm::collect( void )
	{
		lua_gc( state, LUA_GCCOLLECT, 0 );
	}

	up_t lua::vm::size( void ) const
	{
		return lua_gc( state, LUA_GCCOUNT, 0 ) * 1024 + lua_gc( state, LUA_GCCOUNTB, 0 );
	}

	std::string lua::vm::version( void ) const
	{
		lua::stack stack_( state );
		stack_.push_lstring( "_VERSION", 8 );
		stack_.raw_get( globals_index );
		std::string string = stack_.to_lstring( -1 );
		stack_.pop( 1 );
		return string;
	}

	void lua::vm::setup( function_type function )
	{
		function( stack( state ) );
	}
}
