/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "component/registry/registry.hpp"
#include "foundation/executable/program.hpp"

namespace
{
	using namespace ooe;

	bool launch( const std::string&, const std::string&, s32, c8** )
	{
		registry registry;
		registry.insert( "non-existent", module::either );
		registry.find( "non-existent", module::external );
		return true;
	}
}

//--- main ---------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
	return executable::launch( launch, argc, argv );
}
