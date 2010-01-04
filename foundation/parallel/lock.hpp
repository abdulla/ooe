/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_PARALLEL_LOCK_HPP
#define OOE_FOUNDATION_PARALLEL_LOCK_HPP

#include <pthread.h>

#include "foundation/utility/macro.hpp"
#include "foundation/utility/noncopyable.hpp"

namespace ooe
{
	class mutex;
	class condition;
	class read_write;

	class lock;
	class read_lock;
	class write_lock;

//--- mutex --------------------------------------------------------------------
	class OOE_VISIBLE mutex
		: private noncopyable
	{
	public:
		mutex( void );
		virtual ~mutex( void );

	protected:
		pthread_mutex_t pthread_mutex;

		friend class condition;
		friend class lock;
		friend class try_lock;
	};

//--- condition ----------------------------------------------------------------
	class OOE_VISIBLE condition
		: private noncopyable
	{
	public:
		condition( void );
		virtual ~condition( void );

		void wait( lock& );
		void notify_one( void );
		void notify_all( void );

	private:
		pthread_cond_t pthread_cond;
	};

//--- read_write ---------------------------------------------------------------
	class OOE_VISIBLE read_write
		: private noncopyable
	{
	public:
		read_write( void );
		~read_write( void );

	private:
		pthread_rwlock_t pthread_rwlock;

		friend class read_lock;
		friend class write_lock;
		friend class try_read_lock;
		friend class try_write_lock;
	};

//--- lock ---------------------------------------------------------------------
	class OOE_VISIBLE lock
		: private noncopyable
	{
	public:
		lock( ooe::mutex& );
		~lock( void );

	private:
		ooe::mutex& mutex;

		friend class condition;
	};

//--- read_lock ----------------------------------------------------------------
	class OOE_VISIBLE read_lock
		: private noncopyable
	{
	public:
		read_lock( ooe::read_write& );
		~read_lock( void );

	private:
		ooe::read_write& read_write;
	};

//--- write_lock ---------------------------------------------------------------
	class OOE_VISIBLE write_lock
		: private noncopyable
	{
	public:
		write_lock( ooe::read_write& );
		~write_lock( void );

	private:
		ooe::read_write& read_write;
	};
}

#endif	// OOE_FOUNDATION_PARALLEL_LOCK_HPP
