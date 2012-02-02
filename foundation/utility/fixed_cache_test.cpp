/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <limits>

#include "foundation/utility/fixed_cache.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

typedef unit::group< anonymous_t, anonymous_t, 1 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "fixed_cache" );

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

OOE_TEST void fixture_type::test< 0 >( anonymous_t& )
{
    std::cerr << "insert";

    typedef fixed_cache< u32, u32 > cache_type;
    cache_type cache( 16 );

    for ( u32 i = 0; i != 32; ++i )
    {
        cache_type::insert_type inserted = cache.insert( std::make_pair( i, i ) );
        OOE_CHECK( "size: " << cache.size(), cache.size() == std::min( i + 1u, 16u ) );
        OOE_CHECK( "inserted: " << i, inserted.second );
        cache_type::const_iterator found = cache.find( i );
        OOE_CHECK( "found: " << i, found != cache.end() );
        OOE_CHECK( i << " == " << found->first, i == found->first );
        OOE_CHECK( i << " == " << found->second, i == found->second );
    }

    for ( u32 i = 0; i != 16; ++i )
    {
        cache_type::const_iterator found = cache.find( i );
        OOE_CHECK( "not found: " << i, found == cache.end() );
    }

    for ( u32 i = 16; i != 32; ++i )
    {
        cache_type::const_iterator found = cache.find( i );
        OOE_CHECK( "found: " << i, found != cache.end() );
        OOE_CHECK( i << " == " << found->first, i == found->first );
        OOE_CHECK( i << " == " << found->second, i == found->second );
    }
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
