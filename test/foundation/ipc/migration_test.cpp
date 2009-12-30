/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <csignal>

#include <paths.h>

#include "foundation/executable/fork_io.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/ipc/name.hpp"
#include "foundation/ipc/nameservice.hpp"
#include "foundation/ipc/semaphore.hpp"
#include "foundation/ipc/memory/client.hpp"
#include "foundation/ipc/memory/rpc.hpp"
#include "foundation/ipc/memory/server.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

namespace
{
	using namespace ooe;

	class setup
	{
	public:
		setup( void )
			: fork0( 0 ), fork1( 0 )
		{
			ipc::nameservice nameservice;
			nameservice.insert( "migrate", migrate );
			nameservice.insert( "server_pid", server_pid );
			socket_pair pair = make_pair();
			ipc::semaphore semaphore( ipc::unique_name(), ipc::semaphore::create, 0 );

			fork_ptr( new scoped_fork ).swap( fork0 );

			if ( fork0->is_child() )
			{
				OOE_IGNORE
				(
					ipc::memory::server server( "/ooe.test.migration", nameservice );
				 	semaphore.up();
					server_ptr = &server;
					socket_ptr = &pair._0;

					while ( !executable::signal() )
						server.decode();
				);

				fork_io::exit( true );
			}

		 	semaphore.down();
			fork_ptr( new scoped_fork ).swap( fork1 );

			if ( fork1->is_child() )
			{
				OOE_IGNORE
				(
					ipc::memory::server server( ipc::unique_name(), nameservice );
				 	semaphore.up();
					server.relink( pair._1 );

					while ( !executable::signal() )
						server.decode();
				);

				fork_io::exit( true );
			}

		 	semaphore.down();
		}

	private:
		typedef scoped_ptr< scoped_fork > fork_ptr;

		fork_ptr fork0;
		fork_ptr fork1;

		static ipc::memory::server* server_ptr;
		static socket* socket_ptr;

		static void migrate( void )
		{
			server_ptr->migrate( *socket_ptr );
		}

		static pid_t server_pid( void )
		{
			return getpid();
		}
	};

	ipc::memory::server* setup::server_ptr = 0;
	socket* setup::socket_ptr = 0;

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
			void fixture_type::test< 0 >( setup& )
		{
			std::cerr << "migrate between servers\n";

			ipc::memory::client client( "/ooe.test.migration" );
			ipc::memory::call< void ( void ) > migrate( client, "migrate" );
			ipc::memory::call< pid_t ( void ) > server_pid( client, "server_pid" );

			pid_t migrate_pid0 = server_pid();
			migrate();
			pid_t migrate_pid1 = server_pid();

			check( "migrate_pid0 != migrate_pid1", migrate_pid0 != migrate_pid1 );
		}
	}
}
