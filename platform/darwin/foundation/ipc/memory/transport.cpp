/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/environment.hpp"
#include "foundation/io/socket.hpp"
#include "foundation/ipc/memory/transport.hpp"

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
}

namespace ooe
{
//--- platform::ipc::memory::transport -----------------------------------------
	platform::ipc::memory::transport::transport( const std::string& name_, u8 mode )
		: in( name_ + ".i", cast_sem( mode ), 0 ), out( name_ + ".o", cast_sem( mode ), 0 )
	{
	}

	platform::ipc::memory::transport::transport( ooe::socket& socket )
		: in( socket.receive() ), out( socket.receive() )
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
		: platform::ipc::memory::transport( socket ), memory( socket.receive() )
	{
	}

	ipc::memory::transport::~transport( void )
	{
	}

	void ipc::memory::transport::wait( wait_type function, const void* pointer )
	{
		in.down();
		function( get(), size(), pointer );
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

	void ipc::memory::transport::unlink( void )
	{
		in.unlink();
		out.unlink();
		memory.unlink();
	}

	void ipc::memory::transport::migrate( ooe::socket& socket )
	{
		socket.send( in );
		socket.send( out );
		socket.send( memory );
	}
}
