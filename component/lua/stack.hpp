/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_LUA_STACK_HPP
#define OOE_COMPONENT_LUA_STACK_HPP

#include "foundation/utility/fundamental.hpp"
#include "foundation/utility/macro.hpp"

struct lua_State;

OOE_NAMESPACE_BEGIN( ( ooe )( lua )( type ) )

enum id
{
    nil,
    boolean,
    lightuserdata,
    number,
    string,
    table,
    function,
    userdata,
    thread
};

OOE_NAMESPACE_END( ( ooe )( lua )( type ) )

OOE_NAMESPACE_BEGIN( ( ooe )( lua ) )

typedef lua_State state;
typedef s32 ( * cfunction )( state* );
const s32 globals_index = -10002;

//--- stack ----------------------------------------------------------------------------------------
class OOE_VISIBLE stack
{
public:
    stack( lua::state* );

    void pop( u32 );
    u32 size( void ) const;

    type::id type( s32 ) const;
    const c8* type_name( type::id ) const;
    bool raw_equal( s32, s32 ) const;

    f64 to_number( s32 ) const;
    bool to_boolean( s32 ) const;
    const c8* to_lstring( s32, up_t* = 0 ) const;
    up_t objlen( s32 ) const;
    void* to_userdata( s32 ) const;

    void push_nil( void );
    void push_number( f64 );
    void push_lstring( const c8*, up_t );
    void push_cclosure( cfunction, u32 = 0 );
    void push_boolean( bool );
    void push_lightuserdata( const void* );

    void raw_get( s32 );
    void raw_geti( s32, u32 );
    void create_table( u32 = 0, u32 = 0 );
    void* new_userdata( up_t );
    bool get_metatable( s32 );

    void raw_set( s32 );
    void raw_seti( s32, u32 );
    void set_metatable( s32 );

    void error( void );
    bool next( s32 );
    void where( void );

private:
    lua::state* state;
};

//--- upvalue --------------------------------------------------------------------------------------
inline s32 upvalue( u32 index )
{
    return globals_index - index;
}

OOE_NAMESPACE_END( ( ooe )( lua ) )

#endif  // OOE_COMPONENT_LUA_STACK_HPP
