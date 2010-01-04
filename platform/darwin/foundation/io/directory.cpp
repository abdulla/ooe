/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <cerrno>

#include "foundation/io/directory.hpp"
#include "foundation/io/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

//--- directory_open -------------------------------------------------------------------------------
DIR* directory_open( const ooe::descriptor& desc )
{
	DIR* dir = opendir( "/" );

	if ( !dir )
		return dir;
	else if ( close( dir->__dd_fd ) )
		throw error::io( "directory: " ) << "Unable to close \"/\": " << error::number( errno );

	dir->__dd_fd = dup( desc.get() );
	rewinddir( dir );
	return dir;
}

OOE_NAMESPACE_END( ( ooe )( platform ) )