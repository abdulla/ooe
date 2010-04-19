/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/parallel/queue.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

typedef unit::group< anonymous_t, anonymous_t, 2 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "queue" );

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

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
		OOE_CHECK( "queue is not empty", !queue.empty() );
		OOE_CHECK( "queue.dequeue() == " << i, queue.dequeue() == i );
	}
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
