/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <paths.h>

#include "foundation/executable/fork_io.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/ipc/memory/client.hpp"
#include "foundation/ipc/memory/nameservice.hpp"
#include "foundation/ipc/memory/rpc.hpp"
#include "foundation/ipc/memory/server.hpp"
#include "test/unit/assert.hpp"
#include "test/unit/group.hpp"

namespace
{
	using namespace ooe;
	socket_pair* pair_ptr;
	ipc::memory::server* server_ptr;

	void migrate( void )
	{
		server_ptr->migrate( pair_ptr->_0 );
	}

	void null( void )
	{
	}

	struct setup
	{
		setup( void )
		{
			ipc::memory::nameservice nameservice;
			nameservice.insert( "migrate", migrate );
			nameservice.insert( "null", null );

			socket_pair pair = make_pair();
			pair_ptr = &pair;

			fork_io fork0;

			if ( fork0.is_child() )
			{
				ipc::memory::server server( "/ooe.test.migration.0", nameservice );
				server_ptr = &server;

				while ( !executable::signal() )
					server.decode();

				fork_io::exit( true );
			}

			fork_io fork1;

			if ( fork1.is_child() )
			{
				ipc::memory::server server( "/ooe.test.migration.1", nameservice );
				server.relink( pair._1 );

				while ( !executable::signal() )
					server.decode();

				fork_io::exit( true );
			}
		}
	};

	typedef unit::group< setup, empty_t, 1 > group_type;
	typedef group_type::fixture_type fixture_type;
	group_type group( "migration" );
}

namespace ooe
{
	namespace unit
	{
		template<>
		template<>
			void fixture_type::test< 0 >( void )
		{
			std::cerr << "migrate between servers\n";

			ipc::memory::client client( "/ooe.test.migration.0" );
			ipc::memory::call< void ( void ) > migrate( client, "migrate" );
			ipc::memory::call< void ( void ) > null( client, "null" );

			migrate();
			null();
		}
	}
}
