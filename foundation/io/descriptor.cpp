/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <cerrno>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include "foundation/io/descriptor.hpp"
#include "foundation/io/error.hpp"
#include "foundation/utility/convert.hpp"

namespace
{
	using namespace ooe;

	void statistics( s32 fd, struct stat& status )
	{
		if ( fstat( fd, &status ) )
			throw error::io( "descriptor: " ) << "Unable to stat: " << error::number( errno );
	}

	s32 transform( u8 flags )
	{
		if ( ~flags & descriptor::write )
			return O_RDONLY;

		s32 io_flags;

		if ( flags & descriptor::read )
			io_flags = O_RDWR;
		else
			io_flags = O_WRONLY;

		if ( flags & descriptor::truncate )
			io_flags |= O_TRUNC;

		if ( flags & descriptor::append )
			io_flags |= O_APPEND;

		if ( flags & descriptor::create )
			io_flags |= O_CREAT;

		return io_flags;
	}
}

namespace ooe
{
//--- descriptor_id ------------------------------------------------------------
	descriptor_id::descriptor_id( s32 fd_ )
		: fd( fd_ )
	{
		if ( fd == -1 )
			throw error::io( "descriptor: " ) << "Unable to open: " << error::number( errno );
		else if ( fcntl( fd, F_SETFD, FD_CLOEXEC ) )
			throw error::io( "descriptor: " ) <<
				"Unable to set close-on-exec: " << error::number( errno );
	}

	descriptor_id::descriptor_id( const std::string& path, s32 flags )
		: fd( open( path.c_str(), flags, 0600 ) )
	{
		if ( fd == -1 )
			throw error::io( "descriptor: " ) << "Unable to open \"" << path << "\": " <<
				error::number( errno );
		else if ( fcntl( fd, F_SETFD, FD_CLOEXEC ) )
			throw error::io( "descriptor: " ) <<
				"Unable to set close-on-exec for \"" << path << "\": " << error::number( errno );
	}

	descriptor_id::~descriptor_id( void )
	{
		if ( close( fd ) )
			OOE_WARNING( "descriptor", "Unable to close: " << error::number( errno ) );
	}

//--- descriptor ---------------------------------------------------------------
	descriptor::descriptor( s32 fd )
		: id( new descriptor_id( fd ) )
	{
	}

	descriptor::descriptor( const std::string& path, u8 flags )
		: id( new descriptor_id( path, transform( flags ) ) )
	{
	}

	s32 descriptor::get( void ) const
	{
		return id->fd;
	}

	descriptor::node_type descriptor::type( void ) const
	{
		struct stat status;
		statistics( get(), status );

		switch ( status.st_mode & S_IFMT )
		{
		case S_IFDIR:
			return directory;

		case S_IFCHR:
			return character;

		case S_IFBLK:
			return block;

		case S_IFREG:
			return file;

		case S_IFIFO:
			return fifo;

		case S_IFLNK:
			return link;

		case S_IFSOCK:
			return socket;

		default:
			return unknown;
		}
	}

	up_t descriptor::size( void ) const
	{
		struct stat status;
		statistics( get(), status );
		return static_cast< up_t >( status.st_size );
	}

	void descriptor::resize( up_t length )
	{
		if ( ftruncate( get(), length ) )
			throw error::io( "descriptor: " ) << "Unable to resize: " << error::number( errno );
	}

	void descriptor::control( u32 request, void* pointer )
	{
		if ( ioctl( get(), request, pointer ) == -1 )
			throw error::io( "descriptor: " ) << "Unable to control: " << error::number( errno );
	}
}
