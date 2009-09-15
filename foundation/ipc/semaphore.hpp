/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_SEMAPHORE_HPP
#define OOE_FOUNDATION_IPC_MEMORY_SEMAPHORE_HPP

#include <semaphore.h>

#include "foundation/utility/noncopyable.hpp"
#include "foundation/utility/pointer.hpp"
#include "foundation/utility/string.hpp"

namespace ooe
{
	namespace ipc
	{
		class semaphore_id;
		class semaphore;
		class process_lock;
	}

//--- ipc::semaphore_id --------------------------------------------------------
	class ipc::semaphore_id
	{
	public:
		semaphore_id( const std::string& );
		~semaphore_id( void );

	private:
		std::string name;
	};

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

		void unlink( void );

	private:
		scoped_ptr< semaphore_id > id;
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
}

#endif	// OOE_FOUNDATION_IPC_MEMORY_SEMAPHORE_HPP
