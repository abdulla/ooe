/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_SEMAPHORE_HPP
#define OOE_FOUNDATION_IPC_MEMORY_SEMAPHORE_HPP

#include "foundation/ipc/semaphore_forward.hpp"
#include "foundation/utility/noncopyable.hpp"
#include "foundation/utility/pointer.hpp"
#include "foundation/utility/string.hpp"

namespace ooe
{
	namespace ipc
	{
		class semaphore;
		class process_lock;

		class barrier_wait;
		void barrier_notify( const std::string& ) OOE_VISIBLE;
	}

//--- ipc::semaphore -----------------------------------------------------------
	class OOE_VISIBLE ipc::semaphore
	{
	public:
		enum type
		{
			open,
			create
		};

		semaphore( const std::string&, type = open, u32 = 1 );
		~semaphore( void );

		void up( void );
		void down( void );

		std::string name( void ) const;
		void unlink( void );

	private:
		const std::string name_;
		bool unlinkable;
		sem_t* const sem;
	};

//--- ipc::process_lock --------------------------------------------------------
	class OOE_VISIBLE ipc::process_lock
		: private noncopyable
	{
	public:
		process_lock( ipc::semaphore& );
		~process_lock( void );

	private:
		ipc::semaphore& semaphore;
	};

//--- ipc::barrier_wait --------------------------------------------------------
	class OOE_VISIBLE ipc::barrier_wait
	{
	public:
		barrier_wait( const std::string& );
		~barrier_wait( void );

	private:
		ipc::semaphore semaphore;
	};
}

#endif	// OOE_FOUNDATION_IPC_MEMORY_SEMAPHORE_HPP
