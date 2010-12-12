/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <set>
#include <vector>

#include "foundation/parallel/queue.hpp"
#include "foundation/parallel/thread_pool.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

typedef std::vector< result< s32 > > vector_type;
typedef std::set< s32 > set_type;

void enqueue( queue< s32 >& queue, s32 value )
{
    queue.enqueue( value );
}

s32 dequeue( queue< s32 >& queue )
{
    s32 value;
    OOE_CHECK( "queue.dequeue( value )", queue.dequeue( value ) );
    return value;
}

void check_vector( const vector_type& vector )
{
    set_type set;

    for ( vector_type::const_iterator i = vector.begin(), end = vector.end(); i != end; ++i )
        set.insert( ( *i )() );

    OOE_CHECK( set.size() << " == 10000", set.size() == 10000 );
    set_type::const_iterator iterator = set.begin();

    for ( s32 i = 0; i != 10000; ++i, ++iterator )
        OOE_CHECK( *iterator << " == " << i, *iterator == i );
}

typedef unit::group< anonymous_t, anonymous_t, 5 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "queue" );

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

template<>
template<>
    void fixture_type::test< 0 >( anonymous_t& )
{
    std::cerr << "construct and destruct queue\n";

    queue< s32 > queue;
}

template<>
template<>
    void fixture_type::test< 1 >( anonymous_t& )
{
    std::cerr << "enqueue and dequeue items in serial\n";

    queue< s32 > queue;

    for ( s32 i = 0; i != 10000; ++i )
        queue.enqueue( i );

    for ( s32 i = 0, value; i != 10000; ++i )
    {
        OOE_CHECK( "queue.dequeue( value )", queue.dequeue( value ) );
        OOE_CHECK( value << " == " << i, value == i );
    }
}

template<>
template<>
    void fixture_type::test< 2 >( anonymous_t& )
{
    std::cerr << "enqueue items in parallel\n";

    thread_pool pool( "pool" );
    queue< s32 > queue;

    for ( s32 i = 0; i != 10000; ++i )
        async( pool, make_function( enqueue ), queue, i );

    for ( s32 i = 0; i != 10000; ++i )
    {
        for ( s32 value; queue.dequeue( value ); )
            OOE_CHECK( value << " < 10000", value < 10000 );
    }
}

template<>
template<>
    void fixture_type::test< 3 >( anonymous_t& )
{
    std::cerr << "dequeue items in parallel\n";

    thread_pool pool( "pool" );
    queue< s32 > queue;

    for ( s32 i = 0; i != 10000; ++i )
        queue.enqueue( i );

    vector_type vector;

    for ( s32 i = 0; i != 10000; ++i )
        vector.push_back( async( pool, make_function( dequeue ), queue ) );

    check_vector( vector );
}

template<>
template<>
    void fixture_type::test< 4 >( anonymous_t& )
{
    std::cerr << "enqueue and dequeue items in parallel\n";
    std::cerr << "WARNING: This test is currently disabled due to limitations in queue.\n";

#if 0
    thread_pool pool( "pool" );
    queue< s32 > queue;
    vector_type vector;

    for ( s32 j = 0, split_by = 10; j != split_by; ++j )
    {
        for ( s32 i = 0; i != 10000 / split_by; ++i )
            async( pool, make_function( enqueue ), queue, i );

        for ( s32 i = 0; i != 10000 / split_by; ++i )
            vector.push_back( async( pool, make_function( dequeue ), queue ) );
    }

    check_vector( vector );
#endif
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
