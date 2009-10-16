/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/ipc/name.hpp"
#include "foundation/ipc/memory/server.hpp"

namespace
{
	using namespace ooe;

	typedef tuple< const ipc::memory::switchboard&, ipc::memory::write_buffer*, ipc::pool* >
		tuple_type;

	void ipc_decode( u8* buffer_ptr, up_t buffer_size, const void* pointer )
	{
		const tuple_type& args = *static_cast< const tuple_type* >( pointer );
		args._0.execute( buffer_ptr, buffer_size, *args._1, *args._2 );
	}

	void ipc_link( const any& any, u8* buffer_ptr, up_t buffer_size,
		ipc::memory::write_buffer& buffer, ipc::pool& )
	{
		pid_t pid;
		ipc::stream_read< pid_t >::call( header_adjust( buffer ), pid );

		u32 link = static_cast< ipc::memory::server* >( any.pointer )->link( pid );
		up_t size = ipc::stream_size< u32 >::call( link );
		u8* pointer = return_write( buffer_ptr, buffer_size, buffer, size );
		ipc::stream_write< u32 >::call( pointer, link );
	}

	void ipc_unlink( const any& any, u8* buffer_ptr, up_t buffer_size,
		ipc::memory::write_buffer& buffer, ipc::pool& )
	{
		u32 link;
		ipc::stream_read< u32 >::call( header_adjust( buffer ), link );

		static_cast< ipc::memory::server* >( any.pointer )->unlink( link );
		return_write( buffer_ptr, buffer_size, buffer );
	}
}

namespace ooe
{
//--- ipc::memory::servlet -------------------------------------------------------------
	ipc::memory::servlet::servlet( pid_t pid, u32 link_id_, const memory::switchboard& switchboard_,
		server& server )
		: link_id( link_id_ ), transport( link_name( pid, link_id ), transport::create ),
		switchboard( switchboard_ ), listen( new link_listen( link_name( pid, link_id ) ) ),
		active( true ), thread( make_function( *this, &servlet::call ), &server )
	{
	}

	ipc::memory::servlet::~servlet( void )
	{
		ooe::thread self;

		if ( thread == self )
			return;

		active = false;
		stream_write< u32, u32 >::call( transport.get(), true, 0 );
		transport.wake_wait();
		thread.join();
	}

	void* ipc::memory::servlet::call( void* pointer )
	{
		write_buffer buffer( transport.get(), 0, 0 );
		pool pool;
		tuple_type tuple( switchboard, &buffer, &pool );

		memory::server& server = *static_cast< memory::server* >( pointer );
		link_server link( listen->accept(), link_id, server );
		delete listen.release();
		transport.unlink();

		try
		{
			while ( active )
				transport.wait( ipc_decode, &tuple );
		}
		catch ( ipc::migration& migration )
		{
			connect connect( migration.address() );
			transport.migrate( connect );
		}

		return 0;
	}

//--- ipc::memory::server --------------------------------------------------------------
	ipc::memory::server::server( const std::string& name, const switchboard& external_ )
		: semaphore( name + ".s", semaphore::create ), internal(), external( external_ ),
		transport( name, transport::create ), seed(), servlets()
	{
		if ( internal.insert_direct( ipc_link, this ) != 1 )
			throw error::runtime( "ipc::server: " ) << "\"link\" not at index 1";

		if ( internal.insert_direct( ipc_unlink, this ) != 2 )
			throw error::runtime( "ipc::server: " ) << "\"unlink\" not at index 2";
	}

	bool ipc::memory::server::decode( void )
	{
		write_buffer buffer( transport.get(), 0, 0 );
		tuple_type tuple( internal, &buffer, 0 );
		transport.wait( ipc_decode, &tuple );
		return !servlets.empty();
	}

	// note: no need for lock or atomics, semaphore is used when calling link/unlink via decode
	u32 ipc::memory::server::link( pid_t pid )
	{
		u32 link_id = seed++;
		servlet_map::value_type value( link_id, new servlet( pid, link_id, external, *this ) );
		servlets.insert( servlets.end(), value );
		return link_id;
	}

	void ipc::memory::server::unlink( u32 link_id )
	{
		servlets.erase( link_id );
	}
}
