/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <limits>

#include "foundation/utility/binary.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

typedef unit::group< anonymous_t, anonymous_t, 3 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "binary" );

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

template<>
template<>
	void fixture_type::test< 0 >( anonymous_t& )
{
	std::cerr << "is_bit_round\n";

	for ( u16 i = 0, base2 = 1; i != std::numeric_limits< u16 >::max(); ++i )
	{
		if ( i == base2 )
		{
			OOE_CHECK( i << " is a power of 2", is_bit_round( i ) );
			base2 <<= 1;
			continue;
		}

		OOE_CHECK( i << " is not a power of 2", !is_bit_round( i ) );
	}
}

template<>
template<>
	void fixture_type::test< 1 >( anonymous_t& )
{
	std::cerr << "bit_round_up\n";

	for ( u16 i = 0, base2 = 1; i != std::numeric_limits< u16 >::max(); ++i )
	{
		OOE_CHECK( "bit_round_up( " << i << " ) == " << base2, bit_round_up( i ) == base2 );

		if ( i == base2 )
			base2 <<= 1;
	}
}

template<>
template<>
	void fixture_type::test< 2 >( anonymous_t& )
{
	std::cerr << "bit_round_down\n";

	for ( u16 i = 0, base2 = 1; i != std::numeric_limits< u16 >::max(); ++i )
	{
		OOE_CHECK( "bit_round_down( " << i << " ) == " << base2,
			bit_round_down( i ) == ( base2 >> 1 ) );

		if ( i == base2 )
			base2 <<= 1;
	}
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
