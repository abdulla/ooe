/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/executable/program.hpp"
#include "foundation/ipc/nameservice.hpp"
#include "foundation/ipc/memory/server.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

void hello( void )
{
	std::cout << "hello server\n";
}

bool launch( const std::string&, const std::string&, s32 argc, c8** argv )
{
	const c8* up_name = 0;

	for ( s32 option; ( option = getopt( argc, argv, "u:" ) ) != -1; )
	{
		switch ( option )
		{
		case 'u':
			up_name = optarg;
			break;

		default:
			return false;
		}
	}

	ipc::nameservice nameservice;
	nameservice.insert( "hello", hello );

	ipc::memory::server server( "/ooe.hello", nameservice );

	if ( up_name )
		ipc::barrier_notify( up_name );

	while ( !executable::signal() )
		server.decode();

	return true;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

//--- main -----------------------------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
	return executable::launch( launch, argc, argv );
}
