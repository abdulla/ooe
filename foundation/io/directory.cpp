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

namespace
{
	using namespace ooe;

	bool next_entry( s32 fd, up_t& next, up_t& limit, c8* buffer, up_t size )
	{
		dirent* entry;

		do
		{
			if ( next >= limit )
			{
				long base;
				s32 bytes = getdirentries( fd, buffer, size, &base );

				if ( bytes <= 0 )
					return false;

				limit = bytes;
				next = 0;
			}

			entry = reinterpret_cast< dirent* >( buffer + next );
			next += entry->d_reclen;
		}
		while ( entry->d_ino == 0 );	// ignore deleted entries

		return true;
	}
}

namespace ooe
{
//--- directory_id -------------------------------------------------------------
	directory_id::directory_id( void )
		: last( 0 ), next( 0 ), limit( 0 ), buffer()
	{
	}

//--- directory ----------------------------------------------------------------
	directory::directory( const descriptor& desc )
		: descriptor( desc ), id()
	{
		if ( type() == descriptor::directory )
			id = new directory_id;
		else
			throw error::io( "directory: " ) << "Descriptor is not a directory";
	}

	directory::directory( const directory& copy )
		: descriptor( copy ), id( copy.id )
	{
	}

	directory& directory::operator =( const directory& copy )
	{
		static_cast< directory& >( *this ) = copy;
		id = copy.id;
		return *this;
	}

	bool directory::operator ++( void )
	{
		id->last = id->next;
		return next_entry( get(), id->next, id->limit, id->buffer, sizeof( id->buffer ) );
	}

	std::string directory::operator *( void ) const
	{
		return reinterpret_cast< const dirent* >( id->buffer + id->last )->d_name;
	}
}
