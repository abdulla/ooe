/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <cstdlib>

#include "foundation/executable/program.hpp"
#include "test/unit/program.hpp"
#include "test/unit/runner.hpp"

namespace
{
	using namespace ooe;

	bool unit_launch( const std::string&, const std::string& name, s32 argc, c8** argv )
	{
		::signal( SIGCHLD, SIG_DFL );
		time_t time_out = 60;
		bool no_stdout = true;

		for ( s32 option; ( option = getopt( argc, argv, "lst:" ) ) != -1; )
		{
			switch ( option )
			{
			case 'l':
				std::cout << "Available groups of tests:\n";

				for ( unit::runner::iterator_type i = unit::global_runner.begin(),
					end = unit::global_runner.end(); i != end; ++i )
					std::cout << "    " << i->first << '\n';

				return true;

			case 's':
				no_stdout = false;
				break;

			case 't':
				time_out = std::strtoul( optarg, 0, 10 );
				break;

			default:
				std::cout <<
					"Usage:\n"
					"    " << name << " [-t time_out] [test_group]...\n"
					"    " << name << " -l\n"
					"\n"
					"Options:\n"
					"    -s             Enable stdout in tests\n"
					"    -t <time_out>  Time out for each test\n"
					"    -l             List all groups of tests\n";

				return false;
			}
		}

		if ( optind == argc )
			return unit::global_runner.run( time_out, no_stdout );

		bool success = true;

		for ( s32 i = optind; i != argc; ++i )
		{
			if ( !unit::global_runner.run( argv[ i ], time_out, no_stdout ) )
				success = false;
		}

		return success;
	}
}

namespace ooe
{
	namespace unit
	{
		runner global_runner;
	}

	s32 unit::launch( s32 argc, c8** argv )
	{
		return executable::launch( unit_launch, argc, argv );
	}
}
