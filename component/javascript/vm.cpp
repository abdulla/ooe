/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "component/javascript/vm.hpp"

namespace ooe
{
//--- javascript::vm -----------------------------------------------------------
	javascript::vm::vm( void )
		: context( v8::Context::New() )
	{
	}

	javascript::vm::~vm( void )
	{
		context.Dispose();
	}
}
