/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/parallel/queue.hpp"
#include "foundation/parallel/thread_pool.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

void insert( queue< s32 >& queue, s32 value )
{
    queue.enqueue( value );
}

typedef unit::group< anonymous_t, anonymous_t, 3 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "queue" );

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

template<>
template<>
    void fixture_type::test< 0 >( anonymous_t& )
{
    std::cerr << "construct/destruct queue\n";

    queue< s32 > queue;
}

template<>
template<>
    void fixture_type::test< 1 >( anonymous_t& )
{
    std::cerr << "insert items in to queue\n";

    queue< s32 > queue;

    for ( s32 i = 0; i != 10000; ++i )
        queue.enqueue( i );

    for ( s32 i = 0; i != 10000; ++i )
    {
        s32 value = 0;
        OOE_CHECK( "queue.dequeue( value )", queue.dequeue( value ) );
        OOE_CHECK( value << " == " << i, value == i );
    }
}

template<>
template<>
    void fixture_type::test< 2 >( anonymous_t& )
{
    std::cerr << "insert items from thread_pool in to queue\n";

    queue< s32 > queue;
    thread_pool pool( "pool" );

    for ( s32 i = 0; i != 10000; ++i )
        async( pool, make_function( insert ), queue, i );

    for ( s32 i = 0; i != 10000; ++i )
    {
        for ( s32 value; queue.dequeue( value ); )
            OOE_CHECK( value << " < 10000", value < 10000 );
    }
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
