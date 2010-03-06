/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_SEMAPHORE_HPP
#define OOE_FOUNDATION_IPC_SEMAPHORE_HPP

#include "foundation/ipc/semaphore_forward.hpp"
#include "foundation/utility/noncopyable.hpp"
#include "foundation/utility/pointer.hpp"
#include "foundation/utility/string.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

class transport;

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )

OOE_NAMESPACE_BEGIN( ( ooe )( ipc ) )

//--- semaphore ------------------------------------------------------------------------------------
class OOE_VISIBLE semaphore
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
	std::string name_;
	bool unlinkable;
	sem_t* const sem;

	friend class memory::transport;
};

//--- process_lock ---------------------------------------------------------------------------------
class OOE_VISIBLE process_lock
	: private noncopyable
{
public:
	process_lock( ipc::semaphore& );
	~process_lock( void );

private:
	ipc::semaphore& semaphore;
};

//--- barrier_wait ---------------------------------------------------------------------------------
class OOE_VISIBLE barrier_wait
{
public:
	barrier_wait( const std::string& );
	~barrier_wait( void );

private:
	ipc::semaphore semaphore;
};

//--- barrier_notify -------------------------------------------------------------------------------
void barrier_notify( const std::string& ) OOE_VISIBLE;

OOE_NAMESPACE_END( ( ooe )( ipc ) )

#endif	// OOE_FOUNDATION_IPC_SEMAPHORE_HPP
