/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <cerrno>

#include <fcntl.h>
#include <unistd.h>

#include "foundation/io/error.hpp"
#include "foundation/io/file.hpp"

namespace ooe
{
//--- file ---------------------------------------------------------------------
	file::file( const descriptor& desc )
		: descriptor( desc )
	{
		if ( type() != descriptor::file && type() != descriptor::character )
			throw error::io( "file: " ) << "Descriptor is not a file or character device";
	}

	void file::sync( void ) const
	{
		if ( fsync( get() ) )
			throw error::io( "file: " ) << "File could not be synced: " << error::number( errno );
	}

	up_t file::read( void* buffer, up_t bytes )
	{
		sp_t read_ = ::read( get(), buffer, bytes );

		if ( read_ == -1 )
			throw error::io( "file: " ) << "Unable to read: " << error::number( errno );

		return read_;
	}

	up_t file::write( const void* buffer, up_t bytes )
	{
		sp_t wrote = ::write( get(), buffer, bytes );

		if ( wrote == -1 )
			throw error::io( "file: " ) << "Unable to write: " << error::number( errno );

		return wrote;
	}

	void file::seek( sp_t offset, seek_type point )
	{
		s32 whence = SEEK_CUR;

		if ( point == begin )
			whence = SEEK_SET;
		else if ( point == end )
			whence = SEEK_END;

		if ( lseek( get(), offset, whence ) == -1 )
			throw error::io( "file: " ) << "Unable to seek: " << error::number( errno );
	}

	up_t file::tell( void ) const
	{
		sp_t told = static_cast< s32 >( lseek( get(), 0, SEEK_CUR ) );

		if ( told == -1 )
			throw error::io( "file: " ) << "Unable to tell: " << error::number( errno );

		return told;
	}
}
