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

OOE_TEST void fixture_type::test< 0 >( anonymous_t& )
{
    std::cerr << "insert";

    bin bin( 1024, 1024 );
    bin::insert_type result = bin.insert( 1025, 1025 );
    OOE_CHECK( "insert( 1025, 1025 )._2 == false", !result._2 );
    result = bin.insert( 1025, 1024 );
    OOE_CHECK( "insert( 1025, 1024 )._2 == false", !result._2 );
    result = bin.insert( 1024, 1025 );
    OOE_CHECK( "insert( 1024, 1025 )._2 == false", !result._2 );

    result = bin.insert( 1024, 1024 );
    OOE_CHECK( "insert( 1024, 1024 )._0 == 0", result._0 == 0 );
    OOE_CHECK( "insert( 1024, 1024 )._1 == 0", result._1 == 0 );
    OOE_CHECK( "insert( 1024, 1024 )._2 == true", result._2 );

    result = bin.insert( 1024, 1024 );
    OOE_CHECK( "consecutive insert( 1024, 1024 )._2 == false", !result._2 );
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
