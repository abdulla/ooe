/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <csignal>

#include "component/lua/script.hpp"
#include "component/registry/local.hpp"
#include "component/registry/registry.hpp"
#include "component/registry/remote.hpp"
#include "foundation/executable/fork_io.hpp"
#include "foundation/executable/library.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/ipc/name.hpp"
#include "foundation/ipc/semaphore.hpp"
#include "foundation/utility/scoped.hpp"
#include "test/unit/group.hpp"

namespace
{
	using namespace ooe;

	class setup
	{
	public:
		setup( void )
			: path_( executable::path()._0 ), fork( 0 )
		{
			std::string name = ipc::unique_name();
			ipc::semaphore semaphore( name, ipc::semaphore::create, 0 );
			fork_ptr( new fork_io ).swap( fork );

			if ( fork->is_child() )
			{
				OOE_IGNORE
				(
					executable::null_fd( STDOUT_FILENO );
					fork_io::execute( path_ + "registry", "-u", name.c_str(), NULL );
				);

				fork_io::exit( true );
			}

			semaphore.down();
		}

		~setup( void )
		{
			fork->signal( SIGKILL );
		}

		std::string path() const
		{
			return path_;
		}

	private:
		typedef scoped_ptr< ooe::fork_io > fork_ptr;

		std::string path_;
		fork_ptr fork;
	};

	typedef unit::group< setup, empty_t, 5 > group_type;
	typedef group_type::fixture_type fixture_type;
	group_type group( "registry" );
}

namespace ooe
{
	namespace unit
	{
		template<>
		template<>
			void fixture_type::test< 0 >( setup& )
		{
			std::cerr << "search registry for an interface\n";

			interface interface;
			interface.insert< void ( void ) >( "hello" );

			registry registry;
			registry.find( interface );
		}

		template<>
		template<>
			void fixture_type::test< 1 >( setup& setup )
		{
			std::cerr << "load library in-process\n";

			std::string path = setup.path() + "../library/libhello" + library::suffix;

			local local( path );
			local.find< void ( * )( void ) >( "hello" )();
		}

		template<>
		template<>
			void fixture_type::test< 2 >( setup& setup )
		{
			std::cerr << "insert and load library as surrogate\n";

			std::string path = setup.path() + "../library/libhello" + library::suffix;

			registry registry;
			registry.insert( registry::library, path );
			std::string name = registry.surrogate( path );
			remote remote( name );
			remote.find< void ( void ) >( "hello" )();
		}

		template<>
		template<>
			void fixture_type::test< 3 >( setup& setup )
		{
			std::cerr << "insert and load module from server\n";

			std::string path = setup.path() + "hello";
			std::string name = ipc::unique_name();

			ipc::semaphore semaphore( name, ipc::semaphore::create, 0 );
			fork_io fork;

			if ( fork.is_child() )
				fork_io::execute( path, "-u", name.c_str(), NULL );

			scoped< void ( s32 ) >
				scoped( function< void ( s32 ) >( fork, &fork_io::signal ), SIGKILL );
			semaphore.down();
			registry registry;
			registry.insert( registry::server, "/ooe.hello" );
			remote remote( "/ooe.hello" );
			remote.find< void ( void ) >( "hello" )();
		}

		template<>
		template<>
			void fixture_type::test< 4 >( setup& )
		{
			std::cerr << "load module in to lua\n";
		}
	}
}
