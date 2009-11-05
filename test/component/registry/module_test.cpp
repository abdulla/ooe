/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <csignal>

#include "component/registry/builder.hpp"
#include "component/registry/ipc.hpp"
#include "test/unit/assert.hpp"
#include "test/unit/group.hpp"

namespace
{
	using namespace ooe;

	void callback( void )
	{
	}

	typedef unit::group< empty_t, empty_t, 1 > group_type;
	typedef group_type::fixture_type fixture_type;
	group_type group( "module" );
}

namespace ooe
{
	namespace unit
	{
		template<>
		template<>
			void fixture_type::test< 0 >( empty_t& )
		{
			std::cerr << "build module\n";

			module module( module::server, "/ooe.module.test" );
			scoped_ptr< facade::ipc > ipc_ptr( new facade::ipc );

			builder< facade::ipc > builder( module, *ipc_ptr );
			builder.insert( "callback", callback );
		}

	}
}
