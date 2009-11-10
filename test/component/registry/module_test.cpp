/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <csignal>

#include "component/registry/builder.hpp"
#include "component/registry/local.hpp"
#include "component/registry/remote.hpp"
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

			module module;
			scoped_ptr< facade::local > local_ptr( new facade::local );
			scoped_ptr< facade::remote > remote_ptr( new facade::remote );

			builder< facade::remote > builder( module, *local_ptr, *remote_ptr );
			builder.insert( "callback", callback );

			module.insert( "local", local_ptr );
			module.insert( "remote", remote_ptr );
		}

	}
}
