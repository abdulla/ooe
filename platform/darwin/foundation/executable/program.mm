/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <Foundation/Foundation.h>
#include <mach-o/dyld.h>

#include "foundation/executable/program.hpp"
#include "foundation/utility/error.hpp"

namespace ooe
{
//--- executable ---------------------------------------------------------------
	bool executable::path( c8* buffer, up_t size )
	{
		u32 length = size;
		return !_NSGetExecutablePath( buffer, &length );
	}

//--- platform -----------------------------------------------------------------
	bool platform::launch( executable::launch_type launch,
		const std::string& root, const std::string& name, s32 argc, c8** argv )
	{
		bool status = false;
		NSAutoreleasePool* pool = [ [ NSAutoreleasePool alloc ] init ];

		if ( !pool )
			throw error::runtime( "executable: " ) << "Unable to initialise auto-release pool";

		@try
		{
			status = launch( root, name, argc, argv );
		}
		@catch ( NSException* error )
		{
			throw error::runtime( error.name.UTF8String ) << ": " << error.reason.UTF8String;
		}

		[ pool release ];
		return status;
	}
}

