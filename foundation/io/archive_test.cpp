/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cstring>

#include "foundation/executable/program.hpp"
#include "foundation/io/archive.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

typedef unit::group< anonymous_t, anonymous_t, 1 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "archive" );

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

OOE_TEST void fixture_type::test< 0 >( anonymous_t& )
{
    std::cerr << "iterate through an archive\n";

    std::string path = executable::path()._0;
    archive archive( descriptor( path + "../share/test/archive.zip" ) );
    archive::file stored = archive.open( "stored" );
    archive::file deflated = archive.open( "deflated" );

    OOE_CHECK( "sizes of match", stored.size() == deflated.size() );
    OOE_CHECK( "data matches", !std::memcmp( stored.get(), deflated.get(), stored.size() ) );

    up_t j = 1;

    for ( archive::iterator i = archive.begin(), end = archive.end(); i != end; ++i, ++j )
        std::cout << j << ": " << *i << '\n';
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
