/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/executable/environment.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/executable/timer.hpp"
#include "foundation/io/file.hpp"
#include "foundation/image/png.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

//--- launch ---------------------------------------------------------------------------------------
bool launch( const std::string&, const std::string&, s32 argc, c8** argv )
{
    if ( argc != 2 )
    {
        std::cout << "Usage: " << argv[ 0 ] << " <png-image>\n";
        return false;
    }

    timer timer;

    u8 buffer[ executable::static_page_size ];
    file file( ( descriptor( argv[ 1 ] ) ) );
    while ( file.read( buffer, sizeof( buffer ) ) == sizeof( buffer ) ) {}

    f32 elapsed = timer.elapsed();
    std::cout << "Read:   " << elapsed << " secs\n";
    timer.update();

    reader_ptr reader = png::open( descriptor( argv[ 1 ] ) );
    while ( reader->decode_row() ) {}

    elapsed = timer.elapsed();
    std::cout << "Decode: " << elapsed << " secs\n";
    return true;
}

OOE_ANONYMOUS_END( ( ooe ) )

//--- main -----------------------------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
    return executable::launch( launch, argc, argv );
}
