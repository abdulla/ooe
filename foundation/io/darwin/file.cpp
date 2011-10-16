/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cerrno>

#include <fcntl.h>

#include "foundation/io/error.hpp"
#include "foundation/io/file.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- file -----------------------------------------------------------------------------------------
void file::sync( void ) const
{
    if ( fcntl( get(), F_FULLFSYNC ) )
        throw error::io( "file: " ) << "File could not be synced: " << error::number( errno );
}

OOE_NAMESPACE_END( ( ooe ) )
