/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <paths.h>

#include "foundation/io/file.hpp"
#include "foundation/io/socket.hpp"
#include "foundation/parallel/thread.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

const c8 path[] = _PATH_TMP "ooe.test.file";

typedef unit::group< anonymous_t, anonymous_t, 2 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "socket" );

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

OOE_TEST( 0 )
{
    std::cerr << "send/receive descriptor";

    socket_pair pair = make_pair();

    u32 value = 0xdeadbeef;
    descriptor desc( path, descriptor::read_write | descriptor::truncate );
    file( desc ).write( &value, sizeof( value ) );
    pair._0.send( desc );

    value = 0;
    file file( pair._1.receive() );
    file.seek( 0, file::begin );
    file.read( &value, sizeof( value ) );

    OOE_CHECK( value == 0xdeadbeef );
}

OOE_TEST( 1 )
{
    std::cerr << "internet query for localhost";

    internet_query query( "localhost", "http" );
    std::cout << "Number of addresses for \"localhost\": " <<
        std::distance( query.begin(), query.end() ) << '\n';
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
