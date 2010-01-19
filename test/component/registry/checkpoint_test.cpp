/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "component/registry/checkpoint.hpp"
#include "foundation/executable/fork_io.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/ipc/nameservice.hpp"
#include "foundation/ipc/memory/client.hpp"
#include "foundation/ipc/memory/rpc.hpp"
#include "foundation/ipc/memory/server.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

typedef scoped_ptr< scoped_fork > fork_ptr;

class setup
{
public:
	setup( void )
		: fork( 0 )
	{
		std::string name = ipc::unique_name();
		ipc::barrier_wait wait( name );
		fork_ptr( new scoped_fork ).swap( fork );

		if ( fork->is_child() )
		{
			OOE_IGNORE( start_server( name ) );
			fork_io::exit( true );
		}
	}

	void signal( void )
	{
		fork_ptr( 0 ).swap( fork );
	}

private:
	fork_ptr fork;

	void start_server( const std::string& name ) const
	{
		ipc::nameservice nameservice;
		nameservice.insert( "quit", &executable::quit );

		ipc::memory::server server( "/ooe.test.checkpoint", nameservice );
		ipc::barrier_notify( name );
		checkpoint checkpoint;

		while ( !executable::signal() )
		{
			if ( !server.decode() )
				checkpoint.update();
		}
	}
};

typedef unit::group< setup, empty_t, 1 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "checkpoint" );

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

template<>
template<>
	void fixture_type::test< 0 >( setup& setup )
{
	std::cerr << "recover from abnormal program termination\n";

	ipc::memory::client( "/ooe.test.checkpoint" );
	setup.signal();

	ipc::memory::client client( "/ooe.test.checkpoint" );
	ipc::memory::call< void ( void ) >( client, "quit" )();
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
