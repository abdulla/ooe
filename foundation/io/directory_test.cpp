/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "foundation/io/directory.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

typedef unit::group< anonymous_t, anonymous_t, 1 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "directory" );

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

OOE_TEST( 0 )
{
    std::cerr << "iterate through a directory";

    directory dir( descriptor( "." ) );
    up_t j = 1;

    for ( directory::iterator i = dir.begin(), end = dir.end(); i != end; ++i, ++j )
        std::cout << j << ": " << *i << '\n';
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
