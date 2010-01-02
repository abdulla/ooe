/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef __USE_BSD
#define __USE_BSD
#endif	// __USE_BSD
#include <dirent.h>
#ifdef __USE_BSD
#undef __USE_BSD
#endif	// __USE_BSD

#include "foundation/io/directory.hpp"
#include "foundation/io/error.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

//--- next_entry -----------------------------------------------------------------------------------
bool next_entry( s32 fd, up_t& next, up_t& limit, c8* data, up_t size )
{
	dirent* entry;

	do
	{
		if ( next >= limit )
		{
			long base;
			s32 bytes = getdirentries( fd, data, size, &base );

			if ( bytes <= 0 )
				return false;

			limit = bytes;
			next = 0;
		}

		entry = reinterpret_cast< dirent* >( data + next );
		next += entry->d_reclen;
	}
	while ( entry->d_ino == 0 );	// ignore deleted entries

	return true;
}

//--- validate -------------------------------------------------------------------------------------
const descriptor& validate( const descriptor& desc )
{
	if ( desc.type() != descriptor::directory )
		throw error::io( "directory: " ) << "Descriptor is not a directory";

	return desc;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- directory::iterator --------------------------------------------------------------------------
directory::iterator::iterator( directory* dir_ )
	: dir( dir_ )
{
}

void directory::iterator::increment( void )
{
	dir->prev = dir->next;

	if ( !next_entry( dir->get(), dir->next, dir->limit, dir->data, sizeof( dir->data ) ) )
		dir = 0;
}

bool directory::iterator::equal( const iterator& other ) const
{
	return dir == other.dir;
}

std::string directory::iterator::dereference( void ) const
{
	return reinterpret_cast< const dirent* >( dir->data + dir->prev )->d_name;
}

//--- directory ------------------------------------------------------------------------------------
directory::directory( const descriptor& desc )
	: descriptor( validate( desc ) ), prev( 0 ), next( 0 ), limit( 0 ), data()
{
}

directory::iterator directory::begin( void )
{
	return this;
}

directory::iterator directory::end( void )
{
	return 0;
}

OOE_NAMESPACE_END( ( ooe ) )
