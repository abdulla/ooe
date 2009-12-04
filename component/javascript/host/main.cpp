/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "component/javascript/facade.hpp"
#include "component/javascript/vm.hpp"
#include "foundation/executable/program.hpp"

namespace
{
	using namespace ooe;

	bool launch( const std::string&, const std::string&, s32 argc, c8** argv )
	{
		javascript::vm vm;
		vm.setup( javascript::component_setup );

		for ( s32 i = 1; i < argc; ++i )
		{
			const c8* name = std::strrchr( argv[ i ], '/' );

			if ( !name )
				continue;

			vm.load( name + 1, descriptor( argv[ i ] ) );
		}

		return true;
	}
}

//--- main ---------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
	return executable::launch( launch, argc, argv );
}
