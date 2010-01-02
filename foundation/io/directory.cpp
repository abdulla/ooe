/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <cerrno>

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

OOE_NAMESPACE_END( ( ooe ) )
