/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "external/memoryrpc/utility.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/executable/timer.hpp"
#include "foundation/ipc/memory/client.hpp"
#include "foundation/ipc/memory/rpc.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

bool launch( const std::string&, const std::string&, s32 argc, c8** argv )
{
	up_t size = parse( argc, argv );

	ipc::memory::client client( "/ooe" );
	ipc::memory::call< void ( up_t ) > set( client, "set_output" );
	ipc::memory::find find( client );
	ipc::memory::call< const c8* ( void ) > call( client, "call_output" );

	set( size );
	timer timer;

	for ( up_t i = 0; i != iteration_limit; ++i )
		call();

	f32 elapsed = timer.elapsed();

	std::cout << elapsed * microsecond_multiply << '\n';
	return true;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

//--- main -----------------------------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
	return executable::launch( launch, argc, argv );
}
