/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <limits>

#include "foundation/utility/binary.hpp"
#include "foundation/utility/convert.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

union pun32
{
    u8 bytes[ 4 ];
    u32 value;
};

typedef unit::group< anonymous_t, anonymous_t, 6 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "binary" );

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

OOE_TEST( 0 )
{
    std::cerr << "is_bit_round";

    for ( u16 i = 0, base2 = 1; i != std::numeric_limits< u16 >::max(); ++i )
    {
        if ( i == base2 )
        {
            OOE_CHECK( is_bit_round( i ) ) << i << " is not a power of 2";
            base2 <<= 1;
            continue;
        }

        OOE_CHECK( !is_bit_round( i ) ) << i << " is a power of 2";
    }
}

OOE_TEST( 1 )
{
    std::cerr << "bit_round_up";

    for ( u16 i = 0, base2 = 1; i != std::numeric_limits< u16 >::max(); ++i )
    {
        OOE_CHECK( bit_round_up( i ) == base2 ) << "bit_round_up( " << i << " ) == " << base2;

        if ( i == base2 )
            base2 <<= 1;
    }
}

OOE_TEST( 2 )
{
    std::cerr << "bit_round_down";

    for ( u16 i = 0, base2 = 1; i != std::numeric_limits< u16 >::max(); ++i )
    {
        OOE_CHECK( bit_round_down( i ) == ( base2 >> 1 ) ) <<
            "bit_round_down( " << i << " ) == " << base2;

        if ( i == base2 )
            base2 <<= 1;
    }
}

OOE_TEST( 3 )
{
    std::cerr << "saturated_shift";

    // shift single byte
    for ( u16 i = 0; i != 32; ++i )
    {
        u32 x = saturated_shift( 1u, i );
        OOE_CHECK( x == 1u << i ) << "saturated_shift( 1u, " << i << " ) == 1u << " << i;
    }

    for ( u16 i = 32; i != 64; ++i )
    {
        u32 x = saturated_shift( 1, i );
        OOE_CHECK( x == 0xffffffff ) << "saturated_shift( 1u, " << i << " ) == 0xffffffff";
    }

    // shift multiple bytes
    for ( u16 i = 0; i != 25; ++i )
    {
        u32 x = saturated_shift( 0xffu, i );
        OOE_CHECK( x == 0xffu << i ) << "saturated_shift( 0xffu, " << i << " ) == 0xffu << " << i;
    }

    for ( u16 i = 25; i != 40; ++i )
    {
        u32 x = saturated_shift( 0xffu, i );
        OOE_CHECK( x == 0xffffffff ) << "saturated_shift( 0xffu, " << i << " ) == 0xffffffff";
    }
}

OOE_TEST( 4 )
{
    std::cerr << "little_endian";

    pun32 pun = { { 0x11, 0x22, 0x33, 0x44 } };
    u32 value = little_endian< u32 >( pun.value );
    OOE_CHECK( value == 0x44332211 ) << "0x" << hex( value ) << " == 0x44332211";
}

OOE_TEST( 5 )
{
    std::cerr << "big_endian";

    pun32 pun = { { 0x44, 0x33, 0x22, 0x11 } };
    u32 value = big_endian< u32 >( pun.value );
    OOE_CHECK( value == 0x44332211 ) << "0x" << hex( value ) << " == 0x44332211";
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
