/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "component/ui/bin.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

typedef unit::group< anonymous_t, anonymous_t, 1 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "bin" );

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

OOE_TEST( 0 )
{
    std::cerr << "insert";

    bin bin( 1024, 1024 );
    OOE_CHECK( bin.insert( 1025, 1025 ) == bin::insert_type( 0, false ) ) << "insert too large";
    OOE_CHECK( bin.insert( 1025, 1025 ) == bin::insert_type( 0, false ) ) << "insert too large";
    OOE_CHECK( bin.insert( 1025, 1024 ) == bin::insert_type( 0, false ) ) << "insert too large";
    OOE_CHECK( bin.insert( 1024, 1025 ) == bin::insert_type( 0, false ) ) << "insert too large";

    bin::insert_type result = bin.insert( 1024, 1024 );
    OOE_CHECK( *result._0 == rect( 0, 0, 1024, 1024 ) && result._1 ) << "insert exact";

    OOE_CHECK( bin.insert( 1024, 1024 ) == bin::insert_type( 0, false ) ) << "insert into full";

    u32 x[] = { 0, 0, 512, 512 };
    u32 y[] = { 0, 512, 0, 512 };
    bin.clear();

    for ( up_t i = 0; i != 4; ++i )
    {
        result = bin.insert( 512, 512 );
        OOE_CHECK( *result._0 == rect( x[ i ], y[ i ], 512, 512 ) && result._1 ) << i;
    }

    OOE_CHECK( bin.insert( 512, 512 ) == bin::insert_type( 0, false ) ) << "insert into full";

    bin.clear();

    for ( up_t i = 0; i != 16; ++i )
        OOE_CHECK( bin.insert( 256, 256 )._1 ) << i << ". inserted";

    OOE_CHECK( bin.insert( 256, 256 ) == bin::insert_type( 0, false ) ) << "insert into full";

    bin.clear();
    OOE_CHECK( bin.insert( 0, 1024 ) == bin::insert_type( 0, false ) ) << "insert 0 width";
    OOE_CHECK( bin.insert( 1024, 0 ) == bin::insert_type( 0, false ) ) << "insert 0 height";
    OOE_CHECK( bin.insert( 0, 0 ) == bin::insert_type( 0, false ) ) << "insert 0 both";
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
