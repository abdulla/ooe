/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cerrno>

#include <unistd.h>

#include "foundation/io/error.hpp"
#include "foundation/io/file.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- file -----------------------------------------------------------------------------------------
void file::sync( void ) const
{
    if ( fsync( get() ) )
        throw error::io( "file: " ) << "File could not be synced: " << error::number( errno );
}

OOE_NAMESPACE_END( ( ooe ) )
