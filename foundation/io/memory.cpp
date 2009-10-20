/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <cerrno>

#include <sys/mman.h>

#include "foundation/io/error.hpp"
#include "foundation/io/memory.hpp"

namespace
{
	using namespace ooe;

	const descriptor& validate( const descriptor& desc )
	{
		if ( desc.type() != descriptor::file && desc.type() != descriptor::character )
			throw error::io( "file: " ) << "Descriptor is not a file or character device";

		return desc;
	}
}

namespace ooe
{
//--- memory_id ----------------------------------------------------------------
	memory_id::memory_id( u8 flags, up_t size_, s32 fd )
		: area( mmap( 0, size_, flags, MAP_SHARED, fd, 0 ) ), size( size_ )
	{
		if ( area == reinterpret_cast< void* >( -1 ) )
			throw error::io( "memory: " ) << "Unable to map memory: " << error::number( errno );
	}

	memory_id::~memory_id( void )
	{
		if ( munmap( area, size ) )
			OOE_WARNING( "memory", "Unable to unmap memory: " << error::number( errno ) );
	}

//--- memory -------------------------------------------------------------------
	memory::memory( const descriptor& desc, u8 flags, up_t size_ )
		: descriptor( validate( desc ) ),
		internal( new memory_id( flags, size_ ? size_ : descriptor::size(), descriptor::get() ) )
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
