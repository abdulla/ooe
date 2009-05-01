/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_LUA_VM_HPP
#define OOE_FOUNDATION_LUA_VM_HPP

#include "foundation/io/descriptor.hpp"
#include "foundation/lua/stack.hpp"

namespace ooe
{
	namespace lua
	{
		class vm;
	}

	class OOE_VISIBLE lua::vm
		: private noncopyable
	{
	public:
		vm( void );
		~vm( void );

		void load( const std::string&, const descriptor& );
		void collect( void );

		u32 size( void ) const;
		lua::stack stack( void ) const;
		std::string version( void ) const;

	private:
		lua::state* state;
	};
}

#endif	// OOE_FOUNDATION_LUA_VM_HPP
