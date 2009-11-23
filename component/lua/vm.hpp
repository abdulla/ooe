/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_LUA_VM_HPP
#define OOE_COMPONENT_LUA_VM_HPP

#include "component/lua/stack.hpp"
#include "foundation/io/descriptor.hpp"

namespace ooe
{
	namespace lua
	{
		class vm;
	}

//--- lua::vm ------------------------------------------------------------------
	class OOE_VISIBLE lua::vm
		: private noncopyable
	{
	public:
		vm( void );
		~vm( void );

		void load( const std::string&, const std::string& );
		void load( const std::string&, const descriptor& );
		void collect( void );

		up_t size( void ) const;
		lua::stack stack( void ) const;
		std::string version( void ) const;

	private:
		lua::state* state;
	};
}

#endif	// OOE_COMPONENT_LUA_VM_HPP
