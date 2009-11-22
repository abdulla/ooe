/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_LUA_ERROR_HPP
#define OOE_COMPONENT_LUA_ERROR_HPP

#include "component/lua/stack.hpp"
#include "foundation/utility/error.hpp"

namespace ooe
{
	namespace error
	{
		struct lua;
	}

	struct OOE_VISIBLE error::lua
		: virtual public runtime
	{
		lua( ooe::lua::stack& stack )
			: runtime( "lua: " )
		{
			*this << stack.to_lstring( -1 );
			stack.pop( 1 );
		}

		lua( const std::string& string_ )
			: runtime( "lua: " )
		{
			*this << string_;
		}

		virtual ~lua( void ) throw()
		{
		}
	};
}

#endif	// OOE_COMPONENT_LUA_ERROR_HPP
