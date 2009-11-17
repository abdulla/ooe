/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/executable/program.hpp"
#include "foundation/ipc/memory/nameservice.hpp"
#include "foundation/ipc/memory/server.hpp"

namespace
{
	using namespace ooe;

	void hello( void )
	{
		std::cout << "hello server\n";
	}

	bool launch( const std::string&, const std::string&, s32 argc, c8** argv )
	{
		for ( s32 option; ( option = getopt( argc, argv, "u:" ) ) != -1; )
		{
			switch ( option )
			{
			case 'u':
				{ ipc::semaphore( optarg ).up(); }
				break;

			default:
				return false;
			}
		}

		ipc::memory::nameservice nameservice;
		nameservice.insert( "hello", hello );

		ipc::memory::server server( "/ooe.hello", nameservice );

		while ( !executable::signal() )
			server.decode();

		return true;
	}
}

//--- main ---------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
	return executable::launch( launch, argc, argv );
}
