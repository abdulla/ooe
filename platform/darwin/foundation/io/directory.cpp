/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cerrno>

#include <fcntl.h>

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

    if ( fcntl( dir->__dd_fd, F_SETFD, FD_CLOEXEC ) )
        throw error::io( "directory: " ) <<
            "Unable to set close-on-exec: " << error::number( errno );

    rewinddir( dir );
    return dir;
}

OOE_NAMESPACE_END( ( ooe )( platform ) )
