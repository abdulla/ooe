/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/environment.hpp"
#include "foundation/io/socket.hpp"
#include "foundation/ipc/memory/transport.hpp"
#include "foundation/utility/error.hpp"

namespace
{
	using namespace ooe;

	inline ipc::semaphore::type cast_sem( u8 mode )
	{
		return mode == ipc::memory::transport::open ?
			ipc::semaphore::open : ipc::semaphore::create;
	}

	inline ipc::shared_memory::type cast_shm( u8 mode )
	{
		return mode == ipc::memory::transport::open ?
			ipc::shared_memory::open : ipc::shared_memory::create;
	}

	inline std::string receive_name( socket& socket )
	{
		u32 size;

		if ( socket.receive( &size, sizeof( size ) ) != sizeof( size ) )
			throw error::runtime( "ipc::memory::transport: " ) << "Unable to receive size";

		std::string name( size, 0 );

		if ( socket.receive( &name[ 0 ], size ) != size )
			throw error::runtime( "ipc::memroy::transport: " ) << "Unable to receive name";

		return name;
	}

	inline void send_name( socket& socket, const std::string& name )
	{
		u32 size = name.size();

		if ( socket.send( &size, sizeof( size ) ) != sizeof( size ) )
			throw error::runtime( "ipc::memory::transport: " ) << "Unable to send size";
		else if ( socket.send( name.c_str(), size ) != size )
			throw error::runtime( "ipc::memory::transport: " ) << "Unable to send name";
	}
}

namespace ooe
{
//--- platform::ipc::memory::transport -----------------------------------------
	platform::ipc::memory::transport::transport( const std::string& name_, u8 mode )
		: in( name_ + ".i", cast_sem( mode ), 0 ), out( name_ + ".o", cast_sem( mode ), 0 )
	{
	}

	platform::ipc::memory::transport::transport( ooe::socket& socket )
		: in( receive_name( socket ) ), out( receive_name( socket ) )
	{
	}

//--- ipc::memory::transport ---------------------------------------------------
	ipc::memory::transport::transport( const std::string& name_, transport::type mode )
		: platform::ipc::memory::transport( name_, mode ),
		memory( name_, cast_shm( mode ), executable::static_page_size )
	{
		BOOST_STATIC_ASSERT( executable::static_page_size > private_size );
	}

	ipc::memory::transport::transport( ooe::socket& socket )
		: platform::ipc::memory::transport( socket ), memory( std::string(), socket.receive() )
	{
	}

	ipc::memory::transport::~transport( void )
	{
	}

	void ipc::memory::transport::wait( wait_type function, const void* pointer )
	{
		in.down();
		function( pointer );
		out.up();
	}

	void ipc::memory::transport::notify( void )
	{
		in.up();
		out.down();
	}

	void ipc::memory::transport::wake_wait( void )
	{
		in.up();
	}

	void ipc::memory::transport::wake_notify( void )
	{
		out.up();
	}

	u8* ipc::memory::transport::get( void ) const
	{
		return memory.as< u8 >();
	}

	up_t ipc::memory::transport::size( void ) const
	{
		return memory.size() - private_size;
	}

	void ipc::memory::transport::migrate( ooe::socket& socket )
	{
		send_name( socket, in.name() );
		send_name( socket, out.name() );
		socket.send( memory );
	}
}
