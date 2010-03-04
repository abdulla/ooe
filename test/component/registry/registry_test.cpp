/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "component/javascript/facade.hpp"
#include "component/javascript/vm.hpp"
#include "component/lua/facade.hpp"
#include "component/lua/vm.hpp"
#include "component/python/facade.hpp"
#include "component/python/vm.hpp"
#include "component/registry/local.hpp"
#include "component/registry/registry.hpp"
#include "component/registry/remote.hpp"
#include "foundation/executable/fork_io.hpp"
#include "foundation/executable/library.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/ipc/name.hpp"
#include "foundation/ipc/semaphore.hpp"
#include "foundation/utility/scoped.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

typedef scoped_ptr< scoped_fork > fork_ptr;

class setup
{
public:
	setup( void )
		: path_( executable::path()._0 ), fork( 0 )
	{
		start_server();
		std::string module_path = path_ + "../library/libhello" + library::suffix;
		registry registry;
		registry.insert( registry::library, module_path );
	}

	~setup( void )
	{
		fork_io fork_killall;

		if ( fork_killall.is_child() )
		{
			OOE_IGNORE( fork_io::execute( "/usr/bin/killall", "registry", NULL ) );
			fork_io::exit( true );
		}
	}

	std::string path() const
	{
		return path_;
	}

private:
	std::string path_;
	fork_ptr fork;

	void start_server( void )
	{
		std::string name = ipc::unique_name();
		ipc::barrier_wait wait( name );
		fork_ptr( new scoped_fork ).swap( fork );

		if ( fork->is_child() )
		{
			executable::null_fd( STDOUT_FILENO );
			OOE_IGNORE( fork_io::execute( path_ + "registry", "-u", name.c_str(), NULL ) );
			fork_io::exit( true );
		}
	}
};

typedef unit::group< setup, anonymous_t, 7 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "registry" );

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

template<>
template<>
	void fixture_type::test< 0 >( setup& )
{
	std::cerr << "search registry for an interface\n";

	interface interface;
	interface.insert< void ( void ) >( "hello" );

	registry registry;
	registry::info_vector vector = registry.find( interface );
	OOE_CHECK( "vector.size()", vector.size() );
}

template<>
template<>
	void fixture_type::test< 1 >( setup& setup )
{
	std::cerr << "load library in-process\n";

	std::string path = setup.path() + "../library/libhello" + library::suffix;

	interface interface;
	interface.insert< void ( void ) >( "hello" );

	local local( path );
	local.find< void ( * )( void ) >( "hello" )();
	OOE_CHECK( "local.supports( interface )", local.supports( interface ) );

	std::cout << "local-doc( hello ): " << local.doc< void ( * )( void ) >( "hello" ) << '\n';
}

template<>
template<>
	void fixture_type::test< 2 >( setup& setup )
{
	std::cerr << "insert and load library as surrogate\n";

	std::string path = setup.path() + "../library/libhello" + library::suffix;

	interface interface;
	interface.insert< void ( void ) >( "hello" );

	registry registry;
	remote remote( registry.surrogate( path ) );
	remote.find< void ( void ) >( "hello" )();
	OOE_CHECK( "remote.supports( interface )", remote.supports( interface ) );

	std::cout << "remote-doc( hello ): " << remote.doc< void ( void ) >( "hello" ) << '\n';
}

template<>
template<>
	void fixture_type::test< 3 >( setup& setup )
{
	std::cerr << "insert and load module from server\n";

	fork_ptr fork( 0 );

	{
		std::string name = ipc::unique_name();
		ipc::barrier_wait wait( name );
		fork_ptr( new scoped_fork ).swap( fork );

		if ( fork->is_child() )
			fork_io::execute( setup.path() + "hello", "-u", name.c_str(), NULL );
	}

	registry registry;
	registry.insert( registry::server, "/ooe.hello" );
	remote remote( "/ooe.hello" );
	remote.find< void ( void ) >( "hello" )();
}

template<>
template<>
	void fixture_type::test< 4 >( setup& setup )
{
	std::cerr << "load module in to lua\n";

	lua::vm vm;
	vm.setup( lua::component_setup );

	std::string path = setup.path() + "../external/hello/script.lua";
	vm.load( "hello/script.lua", path );

	std::cout <<
		"Lua version: " << vm.version() << "\n"
		"Lua VM size: " << vm.size() << " bytes\n";
}

template<>
template<>
	void fixture_type::test< 5 >( setup& setup )
{
	std::cerr << "load module in to javascript\n";

	javascript::vm vm;
	vm.setup( javascript::component_setup );

	std::string path = setup.path() + "../external/hello/script.js";
	vm.load( "hello/script.js", path );

	std::cout <<
		"JavaScript version: " << vm.version() << "\n"
		"JavaScript VM size: " << vm.size() << " bytes\n";
}

template<>
template<>
	void fixture_type::test< 6 >( setup& setup )
{
	std::cerr << "load module in to python\n";

	python::vm vm;
	vm.setup( python::component_setup );

	std::string path = setup.path() + "../external/hello/script.py";
	vm.load( "hello/script.py", path );

	std::cout << "Python version: " << vm.version() << '\n';
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
