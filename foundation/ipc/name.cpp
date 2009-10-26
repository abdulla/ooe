/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

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
	std::string ipc::link_name( pid_t pid, u32 link_id )
	{
		std::string name( "/ooe." );
		return name << hex( pid ) << 'l' << hex( link_id );
	}

	std::string ipc::local_name( const std::string& name )
	{
		return _PATH_TMP + name + ".local";
	}

	std::string ipc::unique_name( void )
	{
		std::string name( "/ooe." );
		return name << hex( getpid() ) << 'r' << hex( seed++ );
	}
}
