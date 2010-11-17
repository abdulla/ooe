/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/fork_io.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/ipc/nameservice.hpp"
#include "foundation/ipc/semaphore.hpp"
#include "foundation/ipc/socket/rpc.hpp"
#include "foundation/ipc/socket/server.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

class setup
{
public:
    setup( void )
        : fork0( 0 ), fork1( 0 ), fork2( 0 )
    {
        ipc::nameservice nameservice;
        nameservice.insert( "migrate", migrate );
        nameservice.insert( "server_pid", server_pid );
        socket_pair pair = make_pair();
        ipc::semaphore semaphore( ipc::unique_name(), ipc::semaphore::create, 0 );

        fork_ptr( new scoped_fork ).swap( fork0 );

        if ( fork0->is_child() )
        {
            OOE_IGNORE( server_0( nameservice, semaphore, pair._0 ) );
            fork_io::exit( true );
        }

        semaphore.down();
        fork_ptr( new scoped_fork ).swap( fork1 );

        if ( fork1->is_child() )
        {
            OOE_IGNORE( server_1( nameservice, semaphore, pair._1 ) );
            fork_io::exit( true );
        }

        semaphore.down();
        fork_ptr( new scoped_fork ).swap( fork2 );

        if ( fork2->is_child() )
        {
            OOE_IGNORE( server_1( nameservice, semaphore, pair._0 ) );
            fork_io::exit( true );
        }

        semaphore.down();
    }

private:
    typedef scoped_ptr< scoped_fork > fork_ptr;

    fork_ptr fork0;
    fork_ptr fork1;
    fork_ptr fork2;

    static ipc::socket::server* server_ptr;
    static socket* socket_ptr;

    static void migrate( void )
    {
        server_ptr->migrate( *socket_ptr );
    }

    static pid_t server_pid( void )
    {
        return getpid();
    }

    void server_0( const ipc::nameservice& nameservice, ipc::semaphore& semaphore, socket& socket )
    {
        std::string local_name = ipc::local_name( "ooe.test.socket-migration" );
        unlink( local_name.c_str() );
        ipc::socket::server server( local_address( local_name ), nameservice );
        server_ptr = &server;
        socket_ptr = &socket;

        semaphore.up();

        while ( !executable::has_signal() )
            server.accept();
    }

    void server_1( const ipc::nameservice& nameservice, ipc::semaphore& semaphore, socket& socket )
    {
        std::string local_name = ipc::local_name( ipc::unique_name() );
        unlink( local_name.c_str() );
        ipc::socket::server server( local_address( local_name ), nameservice );
        server_ptr = &server;
        socket_ptr = &socket;

        semaphore.up();
        server.relink( socket );

        while ( !executable::has_signal() )
            server.accept();
    }
};

ipc::socket::server* setup::server_ptr = 0;
socket* setup::socket_ptr = 0;

typedef unit::group< setup, anonymous_t, 1 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "socket migration" );

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

template<>
template<>
    void fixture_type::test< 0 >( setup& )
{
    std::cerr << "migrate between servers\n";

    ipc::socket::client client( local_address( ipc::local_name( "ooe.test.socket-migration" ) ) );
    ipc::socket::call< void ( void ) > migrate( client, "migrate" );
    ipc::socket::call< pid_t ( void ) > server_pid( client, "server_pid" );

    pid_t migrate_pid0 = server_pid()();
    migrate();
    pid_t migrate_pid1 = server_pid()();
    migrate();
    pid_t migrate_pid2 = server_pid()();

    OOE_CHECK( "different pids after 1st migration", migrate_pid0 != migrate_pid1 );
    OOE_CHECK( "different pids after 2nd migration", migrate_pid1 != migrate_pid2 );
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
