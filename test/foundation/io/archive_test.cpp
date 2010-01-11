/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <cstring>

#include "foundation/executable/program.hpp"
#include "foundation/io/archive.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

typedef unit::group< empty_t, empty_t, 1 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "archive" );

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

template<>
template<>
	void fixture_type::test< 0 >( empty_t& )
{
	std::cerr << "iterate over a directory\n";

	std::string path = executable::path()._0;
	archive archive( descriptor( path + "../resource/test/archive.zip" ) );
	archive::file stored = archive.open( "stored" );
	archive::file deflated = archive.open( "deflated" );

	check( "sizes of match", stored.size() == deflated.size() );
	check( "data matches", !std::memcmp( stored.get(), deflated.get(), stored.size() ) );

	up_t j = 1;

	for ( archive::iterator i = archive.begin(), end = archive.end(); i != end; ++i, ++j )
		std::cout << j << ": " << *i << '\n';
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
