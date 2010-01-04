/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "test/unit/program.hpp"

//--- main ---------------------------------------------------------------------
extern "C" ooe::s32 main( ooe::s32 argc, ooe::c8** argv/*, ooe::c8** envp*/ )
{
	return ooe::unit::launch( argc, argv );
}
