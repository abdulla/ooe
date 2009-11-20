/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "component/lua/script.hpp"
#include "component/registry/builder.hpp"
#include "component/registry/local.hpp"
#include "component/registry/remote.hpp"

namespace
{
	using namespace ooe;

	void hello( void )
	{
		std::cout << "hello library\n";
	}
}

extern "C" ooe::module OOE_VISIBLE module_open( void )
{
	module module;
	scoped_ptr< facade::local > local_ptr( new facade::local );
	scoped_ptr< facade::remote > remote_ptr( new facade::remote );
	scoped_ptr< facade::lua > lua_ptr( new facade::lua );

	builder< facade::remote, facade::lua > builder( module, *local_ptr, *remote_ptr, *lua_ptr );
	builder.insert( "hello", hello );

	module.insert( "local", local_ptr );
	module.insert( "remote", remote_ptr );
	module.insert( "lua", lua_ptr );
	return module;
}
