/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <paths.h>

#include "foundation/io/directory.hpp"
#include "foundation/ipc/name.hpp"
#include "foundation/utility/atom.hpp"
#include "foundation/utility/convert.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

atom< u32 > seed( 0 );

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( ipc ) )

//--- ipc ------------------------------------------------------------------------------------------
std::string unique_name( void )
{
    std::string name( "/ooe." );
    return name << hex( getpid() ) << 'u' << hex( seed++ );
}

std::string local_name( const std::string& name )
{
    return _PATH_TMP + name + ".local";
}

local_address server_address( const std::string& name )
{
    std::string local_name = ipc::local_name( name );

    if ( exists( local_name ) )
        erase( local_name );

    return local_name;
}

OOE_NAMESPACE_END( ( ooe )( ipc ) )
