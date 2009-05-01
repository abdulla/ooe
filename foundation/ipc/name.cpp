/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <paths.h>

#include "foundation/executable/timer.hpp"
#include "foundation/ipc/name.hpp"
#include "foundation/utility/atom.hpp"
#include "foundation/utility/convert.hpp"
#include "foundation/utility/pointer.hpp"

namespace
{
	using namespace ooe;
	atom value( 0 );
}

namespace ooe
{
//--- ipc ----------------------------------------------------------------------
	std::string ipc::link_name( pid_t pid, u32 link_id )
	{
		std::string name;
		return name << "/ooe." << hex( pid ) << 'l' << hex( link_id );
	}

	std::string ipc::local_name( const std::string& name )
	{
		return _PATH_TMP + name + ".local";
	}

	std::string ipc::unique_name( void )
	{
		std::string name;
		return name << "/ooe." << hex( getpid() ) << 'r' << hex( value++ );
	}

	void ipc::unique_name( c8* buffer, up_t size )
	{
		std::snprintf( buffer, size, "/ooe.%xr%x", getpid(), value++ );
	}
}
