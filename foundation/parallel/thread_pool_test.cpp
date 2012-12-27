/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "foundation/parallel/thread_pool.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

s32 increment( s32 value )
{
    return value + 1;
}

void throwable( void )
{
    throw error::runtime( "throwable" );
}

typedef unit::group< anonymous_t, anonymous_t, 4 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "thread_pool" );

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

OOE_TEST( 0 )
{
    std::cerr << "start/stop thread_pool\n";

    thread_pool pool( "pool" );
}

OOE_TEST( 1 )
{
    std::cerr << "insert tasks in to thread_pool\n";

    thread_pool pool( "pool" );
    result< s32 > result_of_2 = async( pool, make_function( increment ), 1 );
    result< s32 > result_of_3 = async( pool, make_function( increment ), 2 );

    OOE_CHECK( result_of_2() == 2 );
    OOE_CHECK( result_of_3() == 3 );
}

OOE_TEST( 2 )
{
    std::cerr << "throw in thread_pool\n";

    thread_pool pool( "pool" );
    result< void > result_of_throw = async( pool, make_function( throwable ) );

    OOE_EXCEPT( "result_of_throw()", error::runtime, result_of_throw() );
}

OOE_TEST( 3 )
{
    std::cerr << "insert large number of tasks in to thread_pool\n";

    thread_pool pool( "pool" );

    for ( s32 i = 0; i != 10000; ++i )
        async( pool, function< void ( void ) >() );
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
