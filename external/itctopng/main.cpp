/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cstring>
#include <iostream>
#include <iterator>

#include "foundation/executable/program.hpp"
#include "foundation/io/file.hpp"
#include "foundation/io/memory.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

bool launch( const std::string&, const std::string&, s32 argc, c8** argv )
{
    if ( argc != 3 )
    {
        std::cout << "Usage: " << argv[ 0 ] << " <input.itc> <output.png>\n";
        return false;
    }

    u8 signature[] = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a };
    memory in( ( descriptor( argv[ 1 ] ) ) );
    u8* begin = in.as< u8 >();
    u8* end = begin + in.size();
    u8* i = end;

    // search from the end to get the largest image
    for ( i -= sizeof( signature ); i >= begin; --i )
    {
        if ( !std::memcmp( i, signature, sizeof( signature ) ) )
            break;
    }

    if ( i < begin )
    {
        std::cout << "File \"" << argv[ 1 ] << "\" does not contain a PNG image\n";
        return false;
    }

    file file( descriptor( argv[ 2 ], descriptor::write_new ) );
    file.write( i, std::distance( i, end ) );
    return true;
}

OOE_ANONYMOUS_END( ( ooe ) )

//--- main -----------------------------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
    return executable::launch( launch, argc, argv );
}
