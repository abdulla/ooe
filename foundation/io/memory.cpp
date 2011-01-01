/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <cerrno>

#include <sys/mman.h>

#include "foundation/io/error.hpp"
#include "foundation/io/memory.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- memory_id ------------------------------------------------------------------------------------
memory_id::memory_id( s32 fd, u8 flags, up_t offset, up_t size_ )
    : size( size_ ), area( mmap( 0, size, flags, MAP_SHARED, fd, offset ) )
{
    if ( area == MAP_FAILED )
        throw error::io( "memory: " ) << "Unable to map memory: " << error::number( errno );
}

memory_id::~memory_id( void )
{
    if ( munmap( area, size ) )
        OOE_CONSOLE( "memory: " << "Unable to unmap memory: " << error::number( errno ) );
}

//--- memory ---------------------------------------------------------------------------------------
memory::memory( const descriptor& desc, u8 flags )
    : internal( new memory_id( desc.get(), flags, 0, desc.size() ) )
{
}

memory::memory( const descriptor& desc, u8 flags, region window )
    : internal( new memory_id( desc.get(), flags, window._0, window._1 ) )
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

void memory::protect( u8 flags, region window )
{
    up_t size_ = size();

    if ( window._0 + window._1 > size_ )
        throw error::io( "memory: " ) << "Invalid region, " << window << " > " << size_;
    else if ( mprotect( as< u8 >() + window._0, window._1, flags ) )
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

OOE_NAMESPACE_END( ( ooe ) )
