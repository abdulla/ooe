/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_SEMAPHORE_FORWARD_HPP
#define OOE_FOUNDATION_IPC_SEMAPHORE_FORWARD_HPP

#include <semaphore.h>

#include "foundation/utility/fundamental.hpp"
#include "foundation/utility/macro.hpp"

namespace ooe
{
	namespace ipc
	{
		class unnamed_semaphore;
	}

//--- ipc::unnamed_semaphore ---------------------------------------------------
	class OOE_VISIBLE ipc::unnamed_semaphore
	{
	public:
		unnamed_semaphore( u32 );
		~unnamed_semaphore( void );

		void up( void );
		void down( void );

	private:
		sem_t sem;
	};
}

#endif	// OOE_FOUNDATION_IPC_SEMAPHORE_FORWARD_HPP
