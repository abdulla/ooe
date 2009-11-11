/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <csignal>

#include "component/registry/registry.hpp"
#include "foundation/executable/fork_io.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/ipc/name.hpp"
#include "foundation/ipc/semaphore.hpp"
#include "test/unit/group.hpp"

namespace
{
	using namespace ooe;

	class setup
	{
	public:
		setup( void )
			: fork( 0 )
		{
			std::string name = ipc::unique_name();
			ipc::semaphore semaphore( name, ipc::semaphore::create, 0 );
			fork_ptr( new fork_io ).swap( fork );

			if ( fork->is_child() )
			{
				OOE_IGNORE
				(
					executable::path_tuple path = executable::path();
					fork_io::execute( path._0 + "registry", "-u", name.c_str(), 0 );
				);

				fork_io::exit( true );
			}

			semaphore.down();
		}

		~setup( void )
		{
			fork->signal( SIGKILL );
		}

	private:
		typedef scoped_ptr< ooe::fork_io > fork_ptr;

		fork_ptr fork;
	};

	typedef unit::group< setup, empty_t, 2 > group_type;
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
			std::cerr << "connect to registry\n";

			registry registry;
		}

		template<>
		template<>
			void fixture_type::test< 1 >( setup& )
		{
			std::cerr << "search registry for an interface\n";

			interface interface;
			interface.insert< void ( const std::string& ) >( "find" );

			registry registry;
			registry.find( interface );
		}
	}
}
