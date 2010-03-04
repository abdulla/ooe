/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <sys/mman.h>

#include "foundation/executable/fork_io.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/ipc/nameservice.hpp"
#include "foundation/ipc/memory/client.hpp"
#include "foundation/ipc/memory/rpc.hpp"
#include "foundation/ipc/memory/server.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

//--- print ----------------------------------------------------------------------------------------
void print( const c8* text )
{
	std::cout << text << '\n';
}

//--- setup ----------------------------------------------------------------------------------------
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

private:
	typedef scoped_ptr< scoped_fork > fork_ptr;

	fork_ptr fork;

	void start_server( const std::string& name ) const
	{
		executable::null_fd( STDOUT_FILENO );
		ipc::nameservice nameservice;
		nameservice.insert( "print", print );

		ipc::memory::server server( "/ooe.test.canary", nameservice );
		ipc::barrier_notify( name );

		while ( !executable::has_signal() )
			server.decode();
	}
};

//--- data -----------------------------------------------------------------------------------------
class data
{
public:
	data( void )
		: client( "/ooe.test.canary" ),
		index( ipc::memory::find( client )( "print", typeid( print ).name() ) ), allocator(),
		buffer( static_cast< ipc::memory::transport& >( client ).get(),
		static_cast< ipc::memory::transport& >( client ).size(), allocator )
	{
	}

	static void unprotect( u8* address, up_t length )
	{
		if ( mprotect( address, length, PROT_READ | PROT_WRITE ) )
			throw error::runtime( "data: " ) << "Unable to unprotect address";
	}

protected:
	ipc::memory::client client;
	ipc::index_t index;
	ipc::shared_allocator allocator;
	ipc::io_buffer buffer;
};

typedef unit::group< setup, data, 2 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "canary" );

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

template<>
template<>
	void fixture_type::test< 0 >( setup& )
{
	std::cerr << "test server canary with in-band data\n";

	ipc::memory::transport& transport = client;
	u8* pointer = transport.get();
	unprotect( pointer + transport.size(), executable::static_page_size );

	ipc::stream_write< ipc::bool_t, ipc::index_t >::call( pointer, true, index );
	up_t j = ipc::stream_size< ipc::bool_t, ipc::index_t >::call( true, index );
	std::memset( pointer + j, 'c', transport.size() - j );

	transport.notify();

	except( "data validation exception", ipc::memory::load_header( buffer ) );
}

template<>
template<>
	void fixture_type::test< 1 >( setup& )
{
	std::cerr << "test server canary out-of-band data\n";

	buffer.allocate( executable::static_page_size );

	if ( buffer.is_internal() )
		throw error::runtime( "Buffer is internal" );

	u8* pointer = allocator.get( 0 );
	unprotect( pointer + executable::static_page_size, executable::static_page_size );
	std::memset( pointer, 'c', executable::static_page_size );

	ipc::memory::transport& transport = client;
	ipc::stream_write< ipc::bool_t, ipc::index_t, std::string >::
		call( transport.get(), false, index, allocator.name() );

	transport.notify();

	except( "data validation exception", ipc::memory::load_header( buffer ) );
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
