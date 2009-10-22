/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <csignal>

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

	pid_t server_pid( void )
	{
		return getpid();
	}

	class setup
	{
	public:
		setup( void )
			: fork0( 0 ), fork1( 0 )
		{
			ipc::memory::nameservice nameservice;
			nameservice.insert( "migrate", migrate );
			nameservice.insert( "server_pid", server_pid );

			socket_pair pair = make_pair();
			pair_ptr = &pair;

			fork_ptr( new fork_io ).swap( fork0 );

			if ( fork0->is_child() )
			{
				ipc::memory::server server( "/ooe.test.migration.0", nameservice );
				server_ptr = &server;

				while ( !executable::signal() )
					server.decode();

				fork_io::exit( true );
			}

			fork_ptr( new fork_io ).swap( fork1 );

			if ( fork1->is_child() )
			{
				ipc::memory::server server( "/ooe.test.migration.1", nameservice );
				server.relink( pair._1 );

				while ( !executable::signal() )
					server.decode();

				fork_io::exit( true );
			}
		}

		~setup( void )
		{
			fork1->signal( SIGTERM );
			fork0->signal( SIGTERM );
		}

	private:
		typedef scoped_ptr< fork_io > fork_ptr;

		fork_ptr fork0;
		fork_ptr fork1;
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
			ipc::memory::call< pid_t ( void ) > server_pid( client, "server_pid" );

			pid_t migrate_pid0 = server_pid();
			migrate();
			pid_t migrate_pid1 = server_pid();

			assert( "migrate_pid0 != migrate_pid1", migrate_pid0 != migrate_pid1 );
		}
	}
}
