/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <limits>

#include "foundation/utility/binary.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

typedef unit::group< anonymous_t, anonymous_t, 4 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "binary" );

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

OOE_TEST void fixture_type::test< 0 >( anonymous_t& )
{
    std::cerr << "is_bit_round";

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

OOE_TEST void fixture_type::test< 1 >( anonymous_t& )
{
    std::cerr << "bit_round_up";

    for ( u16 i = 0, base2 = 1; i != std::numeric_limits< u16 >::max(); ++i )
    {
        OOE_CHECK( "bit_round_up( " << i << " ) == " << base2, bit_round_up( i ) == base2 );

        if ( i == base2 )
            base2 <<= 1;
    }
}

OOE_TEST void fixture_type::test< 2 >( anonymous_t& )
{
    std::cerr << "bit_round_down";

    for ( u16 i = 0, base2 = 1; i != std::numeric_limits< u16 >::max(); ++i )
    {
        OOE_CHECK( "bit_round_down( " << i << " ) == " << base2,
            bit_round_down( i ) == ( base2 >> 1 ) );

        if ( i == base2 )
            base2 <<= 1;
    }
}

OOE_TEST void fixture_type::test< 3 >( anonymous_t& )
{
    std::cerr << "saturated_shift";

    // shift single byte
    for ( u16 i = 0; i != 32; ++i )
    {
        u32 x = saturated_shift( 1u, i );
        OOE_CHECK( "saturated_shift( 1u, " << i << " ) == 1u << " << i, x == 1u << i );
    }

    for ( u16 i = 32; i != 64; ++i )
    {
        u32 x = saturated_shift( 1, i );
        OOE_CHECK( "saturated_shift( 1u, " << i << " ) == 0xffffffff", x == 0xffffffff );
    }

    // shift multiple bytes
    for ( u16 i = 0; i != 25; ++i )
    {
        u32 x = saturated_shift( 0xffu, i );
        OOE_CHECK( "saturated_shift( 0xffu, " << i << " ) == 0xffu << " << i, x == 0xffu << i );
    }

    for ( u16 i = 25; i != 40; ++i )
    {
        u32 x = saturated_shift( 0xffu, i );
        OOE_CHECK( "saturated_shift( 0xffu, " << i << " ) == 0xffffffff", x == 0xffffffff );
    }
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
