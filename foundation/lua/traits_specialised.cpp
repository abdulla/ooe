/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/lua/traits.hpp"

namespace ooe
{
//--- lua::pointer_base --------------------------------------------------------
	lua::pointer_base::pointer_base( lua::stack& stack_, s32 index )
		: stack( stack_ ), ref()
	{
		stack.pushvalue( index );
		ref = stack.ref();
	}

	lua::pointer_base::~pointer_base( void )
	{
		stack.unref( ref );
	}

	lua::pointer_base::pointer_base( const pointer_base& copy )
		: stack( copy.stack ), ref()
	{
		stack.rawgeti( registry, copy.ref );
		ref = stack.ref();
	}

	lua::pointer_base& lua::pointer_base::operator =( const pointer_base& copy )
	{
		stack = copy.stack;
		stack.rawgeti( registry, copy.ref );

		s32 old_ref = ref;
		ref = stack.ref();
		stack.unref( old_ref );
		return *this;
	}
}
