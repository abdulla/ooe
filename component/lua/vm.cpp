/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

extern "C"
{
    #include <lauxlib.h>
    #include <lualib.h>
}

#include "component/lua/error.hpp"
#include "component/lua/traits.hpp"
#include "component/lua/vm.hpp"
#include "foundation/io/memory.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( lua ) )

//--- vm -------------------------------------------------------------------------------------------
vm::vm( void )
    : state( luaL_newstate() )
{
    lua_cpcall( state, luaopen_base, 0 );
    lua_cpcall( state, luaopen_table, 0 );
    lua_cpcall( state, luaopen_string, 0 );
    lua_cpcall( state, luaopen_math, 0 );
}

vm::~vm( void )
{
    lua_close( state );
}

void vm::load( const std::string& name, const descriptor& desc )
{
    memory memory( desc );

    if ( luaL_loadbuffer( state, memory.as< c8 >(), memory.size(), name.c_str() ) ||
        lua_pcall( state, 0, 0, 0 ) )
    {
        stack stack( state );
        throw error::lua( stack );
    }
}

void vm::collect( void )
{
    lua_gc( state, LUA_GCCOLLECT, 0 );
}

up_t vm::size( void ) const
{
    return lua_gc( state, LUA_GCCOUNT, 0 ) * 1024 + lua_gc( state, LUA_GCCOUNTB, 0 );
}

std::string vm::version( void ) const
{
    stack stack( state );
    push< const c8* >::call( stack, "_VERSION" );
    stack.raw_get( globals_index );

    std::string string;
    to< std::string >::call( stack, string, -1 );
    stack.pop( 1 );

    return string;
}

void vm::setup( function_type function )
{
    function( stack( state ) );
}

OOE_NAMESPACE_END( ( ooe )( lua ) )
