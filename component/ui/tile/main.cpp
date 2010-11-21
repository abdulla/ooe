/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <cstdlib>

#include "component/ui/tile_source.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/io/descriptor.hpp"
#include "foundation/parallel/thread_pool.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

//--- launch ---------------------------------------------------------------------------------------
bool launch( const std::string&, const std::string&, s32 argc, c8** argv )
{
    const c8* directory = 0;
    const c8* file = 0;
    u16 page_size = 256;

    for ( s32 option; ( option = getopt( argc, argv, "d:f:p:" ) ) != -1; )
    {
        switch ( option )
        {
        case 'd':
            directory = optarg;
            break;

        case 'f':
            file = optarg;
            break;

        case 'p':
            page_size = std::strtoul( optarg, 0, 10 );
            break;

        default:
            std::cout <<
                "    -d <path>  Path to directory to store tiles\n"
                "    -f <path>  Path to image to generate tiles from\n"
                "    -p <size>  Page size [default " << page_size << "]\n";

            return false;
        }
    }

    if ( !directory || !file )
    {
        std::cout << "The directory and file paths be specified\n";
        return false;
    }

    thread_pool pool;
    make_tile( descriptor( file ), pool, directory, page_size );
    return true;
}

OOE_ANONYMOUS_END( ( ooe ) )

//--- main -----------------------------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
    return executable::launch( launch, argc, argv );
}
