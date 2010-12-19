/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cerrno>

#include <fcntl.h>

#include "foundation/executable/program.hpp"
#include "foundation/io/directory.hpp"
#include "foundation/io/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

//--- directory_open -------------------------------------------------------------------------------
DIR* directory_open( const ooe::descriptor& desc )
{
    s32 fd = executable::copy_fd( desc.get() );

    if ( fcntl( fd, F_SETFD, FD_CLOEXEC ) )
        throw error::io( "directory: " ) <<
            "Unable to set close-on-exec: " << error::number( errno );

    return fdopendir( fd );
}

OOE_NAMESPACE_END( ( ooe )( platform ) )
