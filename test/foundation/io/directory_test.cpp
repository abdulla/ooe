/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/io/directory.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

typedef unit::group< empty_t, empty_t, 1 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "directory" );

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

template<>
template<>
	void fixture_type::test< 0 >( empty_t& )
{
	std::cerr << "iterate over a directory\n";

	directory dir( descriptor( "." ) );
	up_t j = 1;

	for ( directory::iterator i = dir.begin(), end = dir.end(); i != end; ++i, ++j )
		std::cout << j << ": " << *i << '\n';
}

OOE_NAMESPACE_END( ( ooe )( unit ) )