/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <cerrno>
#include <climits>
#include <cstdlib>

#include <sys/stat.h>

#include "foundation/io/directory.hpp"
#include "foundation/io/error.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

//--- validate -------------------------------------------------------------------------------------
const descriptor& validate( const descriptor& desc )
{
	if ( desc.type() != descriptor::directory )
		throw error::io( "directory: " ) << "Descriptor is not a directory";

	return desc;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- directory_iterator ---------------------------------------------------------------------------
directory_iterator::directory_iterator( DIR* dir_, dirent* entry_ )
	: dir( dir_ ), entry( entry_ )
{
}

void directory_iterator::increment( void )
{
	entry = readdir( dir );
}

bool directory_iterator::equal( const directory_iterator& other ) const
{
	return entry == other.entry;
}

std::string directory_iterator::dereference( void ) const
{
	return entry->d_name;
}

//--- directory ------------------------------------------------------------------------------------
directory::directory( const descriptor& desc )
	: dir( platform::directory_open( validate( desc ) ) )
{
	if ( !dir )
		throw error::io( "directory: " ) << "Unable to open directory: " << error::number( errno );
}

directory::~directory( void )
{
	if ( closedir( dir ) )
		OOE_WARNING( "directory", "Unable to close directory: " << error::number( errno ) );
}

directory::iterator directory::begin( void ) const
{
	rewinddir( dir );
	return iterator( dir, readdir( dir ) );
}

directory::iterator directory::end( void ) const
{
	return iterator( 0, 0 );
}

//--- canonical_path -------------------------------------------------------------------------------
std::string canonical_path( const std::string& path )
{
	c8 buffer[ PATH_MAX ];

	if ( !realpath( path.c_str(), buffer ) )
		throw error::io( "make_directory: " ) <<
			"Unable to canonicalize \"" << path << "\": " << error::number( errno );

	return buffer;
}

//--- make_directory -------------------------------------------------------------------------------
void make_directory( const std::string& path )
{
	if ( mkdir( path.c_str(), 0700 ) )
		throw error::io( "make_directory: " ) <<
			"Unable to make directory \"" << path << "\": " << error::number( errno );
}

//--- exists ---------------------------------------------------------------------------------------
bool exists( const std::string& path )
{
	struct stat status;

	if ( stat( path.c_str(), &status ) && errno == ENOENT )
		return false;

	return true;
}

OOE_NAMESPACE_END( ( ooe ) )
