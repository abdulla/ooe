/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <cerrno>

#include <sys/mman.h>

#include "foundation/io/error.hpp"
#include "foundation/io/memory.hpp"

namespace ooe
{
//--- memory_id ----------------------------------------------------------------
	memory_id::memory_id( u8 flags, up_t size_, s32 fd )
		: size( size_ ), area( mmap( 0, size, flags, MAP_SHARED, fd, 0 ) )
	{
		if ( area == MAP_FAILED )
			throw error::io( "memory: " ) << "Unable to map memory: " << error::number( errno );
	}

	memory_id::~memory_id( void )
	{
		if ( munmap( area, size ) )
			OOE_WARNING( "memory", "Unable to unmap memory: " << error::number( errno ) );
	}

//--- memory -------------------------------------------------------------------
	memory::memory( const descriptor& desc, u8 flags )
		: descriptor( desc ),
		internal( new memory_id( flags, descriptor::size(), descriptor::get() ) )
	{
	}

	// defined so that memory_id::~memory_id() can be hidden
	memory::~memory( void )
	{
	}

	void memory::advise( advise_type advice ) const
	{
		if ( posix_madvise( get(), size(), advice ) )
			throw error::io( "memory: " ) << "Unable to advise: " << error::number( errno );
	}

	void memory::sync( void ) const
	{
		if ( msync( get(), size(), MS_SYNC ) )
			throw error::io( "memory: " ) << "Unable to sync: " << error::number( errno );
	}

	void memory::protect( u8 flags )
	{
		if ( mprotect( get(), size(), flags ) )
			throw error::io( "memory: " ) << "Unable to protect: " << error::number( errno );
	}

	up_t memory::size( void ) const
	{
		return internal->size;
	}

	void* memory::get( void ) const
	{
		return internal->area;
	}
}
