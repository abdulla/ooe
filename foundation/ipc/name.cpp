/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <paths.h>

#include "foundation/ipc/name.hpp"
#include "foundation/utility/atom.hpp"
#include "foundation/utility/convert.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

atom< u32 > seed( 0 );

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( ipc ) )

//--- ipc ------------------------------------------------------------------------------------------
std::string link_name( pid_t pid, time_t time, link_t link )
{
    std::string name( "/ooe." );
    return name << hex( pid ) << 't' << hex( time ) << 'l' << hex( link );
}

std::string local_name( const std::string& name )
{
    return _PATH_TMP + name + ".local";
}

std::string unique_name( void )
{
    std::string name( "/ooe." );
    return name << hex( getpid() ) << 'u' << hex( seed++ );
}

OOE_NAMESPACE_END( ( ooe )( ipc ) )
