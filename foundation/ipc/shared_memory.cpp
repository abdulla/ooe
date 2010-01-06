/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <cerrno>

#include <fcntl.h>
#include <sys/mman.h>

#include "foundation/ipc/error.hpp"
#include "foundation/ipc/shared_memory.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe )( ipc ) )

//--- shm_remove -----------------------------------------------------------------------------------
void shm_remove( const std::string& name )
{
	if ( shm_unlink( name.c_str() ) && errno != OOE_NOEXIST )
		throw error::runtime( "ipc::shared_memory: " ) <<
			"Unable to unlink shared memory \"" << name << "\": " << error::number( errno );
}

//--- shm_create -----------------------------------------------------------------------------------
descriptor shm_create( const std::string& name, bool create, up_t size )
{
	if ( create )
		shm_remove( name );

	s32 flags = create ? O_RDWR | O_CREAT | O_EXCL : O_RDWR;
	descriptor desc( shm_open( name.c_str(), flags, 0600 ) );

	if ( create )
		desc.resize( size );

	return desc;
}

//--- shm_destroy ----------------------------------------------------------------------------------
void shm_destroy( const std::string& name, bool unlinkable )
{
	if ( unlinkable && shm_unlink( name.c_str() ) && errno != OOE_NOEXIST )
		OOE_WARNING( "ipc::shared_memory",
			"Unable to unlink shared memory \"" << name << "\": " << error::number( errno ) );
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe )( ipc ) )

OOE_NAMESPACE_BEGIN( ( ooe )( ipc ) )

//--- shared_memory_base ---------------------------------------------------------------------------
shared_memory_base::shared_memory_base( const std::string& shm_name, bool unlinkable_ )
	: name_( shm_name ), unlinkable( unlinkable_ )
{
}

shared_memory_base::~shared_memory_base( void )
{
	shm_destroy( name_, unlinkable );
}

std::string shared_memory_base::name( void ) const
{
	return name_;
}

void shared_memory_base::unlink( void )
{
	if ( unlinkable )
	{
		shm_remove( name_ );
		name_ = std::string();
		unlinkable = false;
	}
}

//--- shared_memory --------------------------------------------------------------------------------
shared_memory::shared_memory( const std::string& shm_name, type mode, up_t size_ )
	: shared_memory_base( shm_name, mode == create ),
	memory( shm_create( shm_name, mode == create, size_ ), memory::read_write )
{
}

shared_memory::shared_memory( const std::string& shm_name, const ooe::descriptor& desc )
	: shared_memory_base( shm_name, false ), memory( desc, memory::read_write )
{
}

shared_memory::~shared_memory( void )
{
}

//--- locked_memory --------------------------------------------------------------------------------
locked_memory::locked_memory( const std::string& shm_name, type mode, up_t size_ )
	: shared_memory( shm_name, mode, size_ )
{
	if ( mlock( get(), size() ) )
		throw error::runtime( "ipc::locked_memory: " ) <<
			"Unable to lock shared memory \"" << shm_name << "\": " << error::number( errno );
}

locked_memory::~locked_memory( void )
{
	if ( munlock( get(), size() ) )
		OOE_WARNING( "ipc::locked_memory",
			"Unable to unlock shared memory \"" << name() << "\": " << error::number( errno ) );
}

//--- memory_lock ----------------------------------------------------------------------------------
memory_lock::memory_lock( shared_memory& memory_ )
	: memory( memory_ )
{
	if ( mlock( memory.get(), memory.size() ) )
		throw error::runtime( "ipc::memory_lock: " ) << "Unable to lock shared memory \"" <<
			memory.name() << "\": " << error::number( errno );
}

memory_lock::~memory_lock( void )
{
	if ( munlock( memory.get(), memory.size() ) )
		OOE_WARNING( "ipc::memory_lock", "Unable to unlock shared memory \"" <<
			memory.name() << "\": " << error::number( errno ) );
}

OOE_NAMESPACE_END( ( ooe )( ipc ) )
