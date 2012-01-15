/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

extern "C"
{
    #include <lauxlib.h>
}

#include "component/lua/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( lua ) )

//--- stack ----------------------------------------------------------------------------------------
stack::stack( lua::state* state_ )
    : state( state_ )
{
}

void stack::pop( u32 size_ )
{
    lua_settop( state, -1 - size_ );
}

u32 stack::size( void ) const
{
    return lua_gettop( state );
}

type::id stack::type( s32 index ) const
{
    return static_cast< type::id >( lua_type( state, index ) );
}

const c8* stack::type_name( type::id id ) const
{
    return lua_typename( state, id );
}

bool stack::raw_equal( s32 index_1, s32 index_2 ) const
{
    return lua_rawequal( state, index_1, index_2 );
}

f64 stack::to_number( s32 index ) const
{
    return lua_tonumber( state, index );
}

bool stack::to_boolean( s32 index ) const
{
    return lua_toboolean( state, index );
}

const c8* stack::to_lstring( s32 index, up_t* size_ ) const
{
    return lua_tolstring( state, index, size_ );
}

up_t stack::objlen( s32 index ) const
{
    return lua_objlen( state, index );
}

void* stack::to_userdata( s32 index ) const
{
    return lua_touserdata( state, index );
}

void stack::push_nil( void )
{
    lua_pushnil( state );
}

void stack::push_number( f64 number )
{
    lua_pushnumber( state, number );
}

void stack::push_lstring( const c8* string, up_t size_ )
{
    lua_pushlstring( state, string, size_ );
}

void stack::push_cclosure( cfunction function, u32 upvalues )
{
    lua_pushcclosure( state, function, upvalues );
}

void stack::push_boolean( bool boolean )
{
    lua_pushboolean( state, boolean );
}

void stack::push_lightuserdata( const void* pointer )
{
    lua_pushlightuserdata( state, const_cast< void* >( pointer ) );
}

void stack::raw_get( s32 index )
{
    lua_rawget( state, index );
}

void stack::raw_geti( s32 index, u32 array_index )
{
    lua_rawgeti( state, index, array_index );
}

void stack::create_table( u32 array_size, u32 map_size )
{
    lua_createtable( state, array_size, map_size );
}

void* stack::new_userdata( up_t size_ )
{
    return lua_newuserdata( state, size_ );
}

bool stack::get_metatable( s32 index )
{
    return lua_getmetatable( state, index );
}

void stack::raw_set( s32 index )
{
    lua_rawset( state, index );
}

void stack::raw_seti( s32 index, u32 array_index )
{
    lua_rawseti( state, index, array_index );
}

void stack::set_metatable( s32 index )
{
    lua_setmetatable( state, index );
}

void stack::error( void )
{
    lua_error( state );
}

bool stack::next( s32 index )
{
    return lua_next( state, index );
}

void stack::where( void )
{
    luaL_where( state, 1 );
}

OOE_NAMESPACE_END( ( ooe )( lua ) )
