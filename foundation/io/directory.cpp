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
//--- directory ----------------------------------------------------------------
	directory::directory( const descriptor& desc )
		: descriptor( desc ), last( 0 ), next( 0 ), limit( 0 ), buffer()
	{
		if ( type() != descriptor::directory )
			throw error::io( "directory: " ) << "Descriptor is not a directory";
	}

	bool directory::operator ++( void )
	{
		last = next;
		return next_entry( get(), next, limit, buffer, sizeof( buffer ) );
	}

	std::string directory::operator *( void ) const
	{
		return reinterpret_cast< const dirent* >( buffer + last )->d_name;
	}
}
