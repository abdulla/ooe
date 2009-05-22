/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/environment.hpp"
#include "foundation/ipc/semaphore.hpp"
#include "foundation/ipc/shared_memory.hpp"
#include "foundation/ipc/memory/transport.hpp"
#include "foundation/parallel/thread.hpp"
#include "foundation/utility/atom.hpp"

namespace
{
	using namespace ooe;

//--- transport_spinlock -------------------------------------------------------
	class transport_spinlock
		: public ipc::memory::transport
	{
	public:
		transport_spinlock( const std::string&, type );
		virtual ~transport_spinlock( void ) {}

		virtual void wait( wait_type, const void* );
		virtual void notify( void );
		virtual ipc::memory::buffer_tuple get( void ) const;

		virtual void wake_wait( void );
		virtual void wake_notify( void );
		virtual void unlink( void );

	private:
		ipc::semaphore semaphore;
		ipc::shared_memory memory;
	};

	transport_spinlock::transport_spinlock( const std::string& name, transport::type mode )
		: semaphore( name, ipc::semaphore::type( mode ), 0 ),
		memory( name, ipc::shared_memory::type( mode ), executable::static_page_size )
	{
	}

	void transport_spinlock::wait( wait_type function, const void* pointer )
	{
		atom<>& state = *memory.as< atom<> >();

		semaphore.down();
		function( get(), pointer );
		state = false;
		thread::yield();
	}

	void transport_spinlock::notify( void )
	{
		atom<>& state = *memory.as< atom<> >();

		state = true;
		semaphore.up();
		while ( state ) {}
	}

	ipc::memory::buffer_tuple transport_spinlock::get( void ) const
	{
		up_t size = sizeof( atom<> );
		return ipc::memory::buffer_tuple( memory.as< u8 >() + size, memory.size() - size );
	}

	void transport_spinlock::wake_wait( void )
	{
		semaphore.up();
	}

	void transport_spinlock::wake_notify( void )
	{
		atom<>& state = *memory.as< atom<> >();
		state = false;
		thread::yield();
	}

	void transport_spinlock::unlink( void )
	{
		semaphore.unlink();
		memory.unlink();
	}

//--- transport_semaphore ------------------------------------------------------
	class transport_semaphore
		: public ipc::memory::transport
	{
	public:
		transport_semaphore( const std::string&, type );
		virtual ~transport_semaphore( void ) {}

		virtual void wait( wait_type, const void* );
		virtual void notify( void );
		virtual ipc::memory::buffer_tuple get( void ) const;

		virtual void wake_wait( void );
		virtual void wake_notify( void );
		virtual void unlink( void );

	private:
		ipc::semaphore semaphore_in;
		ipc::semaphore semaphore_out;
		ipc::shared_memory memory;
	};

	transport_semaphore::transport_semaphore( const std::string& name, transport::type mode )
		: semaphore_in( name + ".i", ipc::semaphore::type( mode ), 0 ),
		semaphore_out( name + ".o", ipc::semaphore::type( mode ), 0 ),
		memory( name, ipc::shared_memory::type( mode ), executable::static_page_size )
	{
	}

	void transport_semaphore::wait( wait_type function, const void* pointer )
	{
		semaphore_in.down();
		function( get(), pointer );
		semaphore_out.up();
	}

	void transport_semaphore::notify( void )
	{
		semaphore_in.up();
		semaphore_out.down();
	}

	ipc::memory::buffer_tuple transport_semaphore::get( void ) const
	{
		return ipc::memory::buffer_tuple( memory.as< u8 >(), memory.size() );
	}

	void transport_semaphore::wake_wait( void )
	{
		semaphore_in.up();
	}

	void transport_semaphore::wake_notify( void )
	{
		semaphore_out.up();
	}

	void transport_semaphore::unlink( void )
	{
		semaphore_in.unlink();
		semaphore_out.unlink();
		memory.unlink();
	}
}

namespace ooe
{
//--- ipc::memory ----------------------------------------------------------------------
	ipc::memory::transport* ipc::memory::
		create_spinlock( const std::string& name, transport::type mode )
	{
		return new transport_spinlock( name, mode );
	}

	ipc::memory::transport* ipc::memory::
		create_semaphore( const std::string& name, transport::type mode )
	{
		return new transport_semaphore( name, mode );
	}
}
