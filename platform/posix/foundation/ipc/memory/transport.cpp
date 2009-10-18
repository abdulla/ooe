/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/environment.hpp"
#include "foundation/io/socket.hpp"
#include "foundation/ipc/memory/transport.hpp"

namespace
{
	using namespace ooe;

	inline ipc::shared_memory::type cast( u8 mode )
	{
		return mode == ipc::memory::transport::open ?
			ipc::shared_memory::open : ipc::shared_memory::create;
	}
}

namespace ooe
{
//--- platform::ipc::memory::transport -----------------------------------------
	platform::ipc::memory::transport::transport( u8 mode_ )
		: mode( mode_ ), in(), out()
	{
	}

//--- ipc::memory::transport ---------------------------------------------------
	ipc::memory::transport::transport( const std::string& name, transport::type mode_ )
		: platform::ipc::memory::transport( mode_ ),
		memory( name, cast( mode ), executable::static_page_size )
	{
		unnamed_semaphore* pointer = memory.as< unnamed_semaphore >();

		if ( mode == create )
		{
			in = new( pointer + 0 ) unnamed_semaphore( 0 );
			out = new( pointer + 1 ) unnamed_semaphore( 0 );
		}
		else
		{
			in = pointer + 0;
			out = pointer + 1;
		}
	}

	ipc::memory::transport::transport( ooe::socket& socket )
		: platform::ipc::memory::transport( shared_memory::open ), memory( socket.receive() )
	{
	}

	ipc::memory::transport::~transport( void )
	{
		unnamed_semaphore* pointer = memory.as< unnamed_semaphore >();

		if ( mode == create )
		{
			( pointer + 0 )->~unnamed_semaphore();
			( pointer + 1 )->~unnamed_semaphore();
		}
	}

	void ipc::memory::transport::wait( wait_type function, const void* pointer )
	{
		in->down();
		function( get(), size(), pointer );
		out->up();
	}

	void ipc::memory::transport::notify( void )
	{
		in->up();
		out->down();
	}

	void ipc::memory::transport::wake_wait( void )
	{
		in->up();
	}

	void ipc::memory::transport::wake_notify( void )
	{
		out->up();
	}

	u8* ipc::memory::transport::get( void ) const
	{
		return memory.as< u8 >() + sizeof( unnamed_semaphore ) * 2;
	}

	up_t ipc::memory::transport::size( void ) const
	{
		return memory.size() - sizeof( unnamed_semaphore ) * 2;
	}

	void ipc::memory::transport::unlink( void )
	{
		memory.unlink();
	}

	void ipc::memory::transport::migrate( ooe::socket& socket )
	{
		socket.send( memory.desc() );
		mode = open;
	}
}
