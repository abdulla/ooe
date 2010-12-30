/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_LUA_VM_HPP
#define OOE_COMPONENT_LUA_VM_HPP

#include "component/lua/stack.hpp"
#include "foundation/io/descriptor.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( lua ) )

//--- vm -------------------------------------------------------------------------------------------
class OOE_VISIBLE vm
    : private noncopyable
{
public:
    typedef void ( * function_type )( stack );

    vm( void );
    ~vm( void );

    void load( const std::string&, const descriptor& );
    void collect( void );

    up_t size( void ) const;
    std::string version( void ) const;

    void setup( function_type );

private:
    lua::state* state;
};

OOE_NAMESPACE_END( ( ooe )( lua ) )

#endif  // OOE_COMPONENT_LUA_VM_HPP
