/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

extern "C"
{
	#include <lauxlib.h>
	#include <lualib.h>
}

#include "component/lua/error.hpp"
#include "component/lua/vm.hpp"
#include "foundation/io/memory.hpp"

namespace
{
	using namespace ooe;

	void vm_load( lua::state* state, const c8* string, up_t size, const c8* name )
	{
		if ( luaL_loadbuffer( state, string, size, name ) || lua_pcall( state, 0, 0, 0 ) )
		{
			lua::stack stack( state );
			throw error::lua( stack );
		}
	}
}

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

	void lua::vm::load( const std::string& name, const std::string& script )
	{
		vm_load( state, script.c_str(), script.size(), name.c_str() );
	}

	void lua::vm::load( const std::string& name, const descriptor& desc )
	{
		memory memory( desc );
		vm_load( state, memory.as< c8 >(), memory.size(), name.c_str() );
	}

	void lua::vm::collect( void )
	{
		lua_gc( state, LUA_GCCOLLECT, 0 );
	}

	up_t lua::vm::size( void ) const
	{
		return lua_gc( state, LUA_GCCOUNT, 0 ) * 1024 + lua_gc( state, LUA_GCCOUNTB, 0 );
	}

	lua::stack lua::vm::stack( void ) const
	{
		return state;
	}

	std::string lua::vm::version( void ) const
	{
		lua::stack stack_ = stack();
		stack_.push_lstring( "_VERSION", 8 );
		stack_.raw_get( globals_index );
		std::string string = stack_.to_lstring( -1 );
		stack_.pop( 1 );
		return string;
	}
}
