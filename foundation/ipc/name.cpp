/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <paths.h>

#include "foundation/ipc/name.hpp"
#include "foundation/utility/atom.hpp"
#include "foundation/utility/convert.hpp"

namespace
{
	using namespace ooe;
	atom< u32 > seed( 0 );
}

namespace ooe
{
//--- ipc ----------------------------------------------------------------------
	std::string ipc::link_name( pid_t pid, time_t time, link_t link )
	{
		std::string name( "/ooe." );
		return name << hex( pid ) << 't' << hex( time ) << 'l' << hex( link );
	}

	std::string ipc::local_name( const std::string& name )
	{
		return _PATH_TMP + name + ".local";
	}

	std::string ipc::unique_name( void )
	{
		std::string name( "/ooe." );
		return name << hex( getpid() ) << 'u' << hex( seed++ );
	}
}
