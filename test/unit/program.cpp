/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <cstdlib>

#include "foundation/executable/program.hpp"
#include "test/unit/program.hpp"
#include "test/unit/runner.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe )( unit ) )

bool unit_launch( const std::string&, const std::string& name, s32 argc, c8** argv )
{
    ::signal( SIGCHLD, SIG_DFL );
    time_t timeout = 60;
    bool no_stdout = true;

    for ( s32 option; ( option = getopt( argc, argv, "lst:" ) ) != -1; )
    {
        switch ( option )
        {
        case 'l':
            std::cout << "Available groups of tests:\n";

            for ( runner::const_iterator i = global_runner.begin(), end = global_runner.end();
                i != end; ++i )
                std::cout << "    " << i->first << '\n';

            return true;

        case 's':
            no_stdout = false;
            break;

        case 't':
            timeout = std::strtoul( optarg, 0, 10 );
            break;

        default:
            std::cout <<
                "Usage:\n"
                "    " << name << " [-t timeout] [test_group]...\n"
                "    " << name << " -l\n"
                "\n"
                "Options:\n"
                "    -s             Enable stdout in tests\n"
                "    -t <timeout>   Time out for each test\n"
                "    -l             List all groups of tests\n";

            return false;
        }
    }

    if ( optind == argc )
        return global_runner.run( timeout, no_stdout );

    bool success = true;

    for ( s32 i = optind; i != argc; ++i )
    {
        if ( !global_runner.run( argv[ i ], timeout, no_stdout ) )
            success = false;
    }

    return success;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe )( unit ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

runner global_runner;

s32 launch( s32 argc, c8** argv )
{
    return executable::launch( unit_launch, argc, argv );
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
