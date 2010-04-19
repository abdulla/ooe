/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <paths.h>

#include "foundation/parallel/thread_pool.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

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

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

template<>
template<>
	void fixture_type::test< 0 >( anonymous_t& )
{
	std::cerr << "start/stop thread_pool\n";

	thread_pool pool;
}

template<>
template<>
	void fixture_type::test< 1 >( anonymous_t& )
{
	std::cerr << "insert tasks in to thread_pool\n";

	thread_pool pool;
	result< s32 > result_of_2 = async( pool, make_function( increment ), 1 );
	result< s32 > result_of_3 = async( pool, make_function( increment ), 2 );

	OOE_CHECK( "result_of_2() == 2", result_of_2() == 2 );
	OOE_CHECK( "result_of_3() == 3", result_of_3() == 3 );
}

template<>
template<>
	void fixture_type::test< 2 >( anonymous_t& )
{
	std::cerr << "throw in thread_pool\n";

	thread_pool pool;
	result< void > result_of_throw = async( pool, make_function( throwable ) );

	OOE_EXCEPT( "result_of_throw()", error::runtime, result_of_throw() );
}

template<>
template<>
	void fixture_type::test< 3 >( anonymous_t& )
{
	std::cerr << "insert large number of tasks in to thread_pool\n";

	thread_pool pool;

	for ( s32 i = 0; i != 10000; ++i )
		async( pool, make_function( increment ), i );
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
