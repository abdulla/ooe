/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <cerrno>

#include <fcntl.h>
#include <sys/mman.h>

#include "foundation/ipc/error.hpp"
#include "foundation/ipc/shared_memory.hpp"

namespace
{
	using namespace ooe;

	descriptor memory_open( const std::string& name, ipc::shared_memory::type mode, up_t size )
	{
		s32 flags = mode == ipc::shared_memory::open ? O_RDWR : O_RDWR | O_CREAT | O_EXCL;
		descriptor desc( shm_open( name.c_str(), flags, 0600 ) );

		if ( mode == ipc::shared_memory::create )
			desc.resize( size );

		return desc;
	}
}

namespace ooe
{
//--- ipc::shared_memory_id ----------------------------------------------------
	ipc::shared_memory_id::shared_memory_id( const std::string& name_ )
		: name( name_ )
	{
		if ( shm_unlink( name.c_str() ) && errno != OOE_NOEXIST )
			throw error::runtime( "ipc::shared_memory: " ) <<
				"Unable to unlink shared memory: \"" << name << "\": " <<
				error::number( errno ) << '\n';
	}

	ipc::shared_memory_id::~shared_memory_id( void )
	{
		if ( shm_unlink( name.c_str() ) && errno != OOE_NOEXIST )
			OOE_WARNING( "ipc::shared_memory", "Unable to unlink shared memory \"" <<
				name << "\": " << error::number( errno ) );
	}

//--- ipc::shared_memory_base --------------------------------------------------
	ipc::shared_memory_base::shared_memory_base( const std::string& name, bool open )
		: id( open ? 0 : new shared_memory_id( name ) )
	{
	}

	ipc::shared_memory_base::~shared_memory_base( void )
	{
	}

//--- ipc::shared_memory -------------------------------------------------------
	ipc::shared_memory::shared_memory( const std::string& name_, type mode, up_t size_ )
		: shared_memory_base( name_, mode == open ),
		memory( memory_open( name_, mode, size_ ), memory::read_write )
	{
	}

	ipc::shared_memory::~shared_memory( void )
	{
	}

	std::string ipc::shared_memory::name( void ) const
	{
		return shared_memory_base::id ? shared_memory_base::id->name : std::string();
	}

	void ipc::shared_memory::unlink( void )
	{
		delete shared_memory_base::id.release();
	}
}
