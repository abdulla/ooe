/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

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

OOE_ANONYMOUS_BEGIN( ( ooe ) )

typedef scoped_ptr< scoped_fork > fork_ptr;

//--- data -----------------------------------------------------------------------------------------
void spawn( const std::string& executable, const std::string& script )
{
    fork_io fork;

    if ( fork.is_child() )
    {
        OOE_PRINT( "spawn", fork_io::execute( executable, script.c_str(), NULL ) );
        fork_io::exit( false );
    }

    OOE_CHECK( fork.wait() == fork_io::success );
}

//--- setup ----------------------------------------------------------------------------------------
class setup
{
public:
    setup( void )
        : fork( 0 )
    {
        path_ = executable::path()._0;
        start_server();
        registry registry;
        registry.insert( registry::library, path_ + "../lib/libhello" OOE_EXTENSION );
    }

    static const std::string& path( void )
    {
        return path_;
    }

private:
    static std::string path_;
    fork_ptr fork;

    void start_server( void )
    {
        std::string name = ipc::unique_name();
        ipc::barrier_wait wait( name );
        io_triplet io( -1, executable::null_fd(), -1 );
        fork.reset( new scoped_fork( io ) );

        if ( fork->is_child() )
        {
            OOE_PRINT( "registry",
                fork_io::execute( path_ + "registry", "-u", name.c_str(), NULL ) );
            fork_io::exit( false );
        }
    }
};

std::string setup::path_;

typedef unit::group< setup, anonymous_t, 6 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "registry" );

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

OOE_TEST( 0 )
{
    std::cerr << "search registry for an interface and list all modules";

    interface interface;
    interface.insert< void ( void ) >( "hello" );

    registry registry;
    registry::info_vector vector = registry.find( interface );
    OOE_CHECK( vector.size() );
    vector = registry.list_all();
    OOE_CHECK( vector.size() );
}

OOE_TEST( 1 )
{
    std::cerr << "load module in-process";

    std::string path = setup::path() + "../lib/libhello" OOE_EXTENSION;

    interface interface;
    interface.insert< void ( void ) >( "hello" );

    local local( path );
    local.find< void ( * )( void ) >( "hello" )();
    OOE_CHECK( local.supports( interface ) );

    std::cout << "local-doc( hello ): " << local.doc< void ( * )( void ) >( "hello" ) << '\n';
}

OOE_TEST( 2 )
{
    std::cerr << "load module as surrogate";

    std::string path = setup::path() + "../lib/libhello" OOE_EXTENSION;

    interface interface;
    interface.insert< void ( void ) >( "hello" );

    registry registry;
    remote remote( ipc::local_name( registry.surrogate( path ) ) );
    remote.find< void ( void ) >( "hello" )();
    OOE_CHECK( remote.supports( interface ) );

    std::cout << "remote-doc( hello ): " << remote.doc< void ( void ) >( "hello" ) << '\n';
}

OOE_TEST( 3 )
{
    std::cerr << "insert and load module as server";

    fork_ptr fork( 0 );

    {
        std::string name = ipc::unique_name();
        ipc::barrier_wait wait( name );
        fork.reset( new scoped_fork );

        if ( fork->is_child() )
        {
            fork_io::execute( setup::path() + "hello", "-u", name.c_str(), NULL );
            fork_io::exit( true );
        }
    }

    registry registry;
    registry.insert( registry::server, "ooe.hello" );
    remote remote( ipc::local_name( "ooe.hello" ) );
    remote.find< void ( void ) >( "hello" )();
}

OOE_TEST( 4 )
{
    std::cerr << "load module in lua";

    std::string executable = setup::path() + "lua_host";
    std::string script = setup::path() + "../share/test/script.lua";
    spawn( executable, script );
}

OOE_TEST( 5 )
{
    std::cerr << "load module in python";

    std::string executable = setup::path() + "python_host";
    std::string script = setup::path() + "../share/test/script.py";
    spawn( executable, script );
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
