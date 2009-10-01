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
		virtual void wake_wait( void );
		virtual void wake_notify( void );

		virtual u8* get( void ) const;
		virtual up_t size( void ) const;
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
		atom< u32 >& state = *memory.as< atom< u32 > >();

		semaphore.down();
		function( get(), size(), pointer );
		state = false;
		thread::yield();
	}

	void transport_spinlock::notify( void )
	{
		atom< u32 >& state = *memory.as< atom< u32 > >();

		state = true;
		semaphore.up();
		while ( state ) {}
	}

	void transport_spinlock::wake_wait( void )
	{
		semaphore.up();
	}

	void transport_spinlock::wake_notify( void )
	{
		atom< u32 >& state = *memory.as< atom< u32 > >();
		state = false;
		thread::yield();
	}

	u8* transport_spinlock::get( void ) const
	{
		return memory.as< u8 >() + sizeof( atom< u32 > );
	}

	up_t transport_spinlock::size( void ) const
	{
		return memory.size() - sizeof( atom< u32 > );
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
		virtual void wake_wait( void );
		virtual void wake_notify( void );

		virtual u8* get( void ) const;
		virtual up_t size( void ) const;
		virtual void unlink( void );

	private:
		ipc::semaphore in;
		ipc::semaphore out;
		ipc::shared_memory memory;
	};

	transport_semaphore::transport_semaphore( const std::string& name, transport::type mode )
		: in( name + ".i", ipc::semaphore::type( mode ), 0 ),
		out( name + ".o", ipc::semaphore::type( mode ), 0 ),
		memory( name, ipc::shared_memory::type( mode ), executable::static_page_size )
	{
	}

	void transport_semaphore::wait( wait_type function, const void* pointer )
	{
		in.down();
		function( get(), size(), pointer );
		out.up();
	}

	void transport_semaphore::notify( void )
	{
		in.up();
		out.down();
	}

	void transport_semaphore::wake_wait( void )
	{
		in.up();
	}

	void transport_semaphore::wake_notify( void )
	{
		out.up();
	}

	u8* transport_semaphore::get( void ) const
	{
		return memory.as< u8 >();
	}

	up_t transport_semaphore::size( void ) const
	{
		return memory.size();
	}

	void transport_semaphore::unlink( void )
	{
		in.unlink();
		out.unlink();
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
