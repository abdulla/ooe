/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "foundation/utility/traits.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

typedef unit::group< anonymous_t, anonymous_t, 2 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "fundamental" );

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

OOE_TEST( 0 )
{
    std::cerr << "types have correct properties";

    OOE_CHECK( "s8 is signed", is_signed< s8 >::value );
    OOE_CHECK( "s16 is signed", is_signed< s16 >::value );
    OOE_CHECK( "s32 is signed", is_signed< s32 >::value );
    OOE_CHECK( "s64 is signed", is_signed< s64 >::value );
    OOE_CHECK( "sp_t is signed", is_signed< sp_t >::value );

    OOE_CHECK( "u8 is unsigned", is_unsigned< u8 >::value );
    OOE_CHECK( "u16 is unsigned", is_unsigned< u16 >::value );
    OOE_CHECK( "u32 is unsigned", is_unsigned< u32 >::value );
    OOE_CHECK( "u64 is unsigned", is_unsigned< u64 >::value );
    OOE_CHECK( "up_t is unsigned", is_unsigned< up_t >::value );

    OOE_CHECK( "f32 is floating point", is_floating_point< f32 >::value );
    OOE_CHECK( "f64 is floating point", is_floating_point< f64 >::value );

    OOE_CHECK( "c8 is integral", is_integral< c8 >::value );
    OOE_CHECK( "c32 is integral", is_integral< c32 >::value );
}

OOE_TEST( 1 )
{
    std::cerr << "types have correct size";

    OOE_CHECK( "s8 is 8 bits", sizeof( s8 ) == 1 );
    OOE_CHECK( "s16 is 16 bits", sizeof( s16 ) == 2 );
    OOE_CHECK( "s32 is 32 bits", sizeof( s32 ) == 4 );
    OOE_CHECK( "s64 is 64 bits", sizeof( s64 ) == 8 );
    OOE_CHECK( "sp_t is pointer sized", sizeof( sp_t ) == sizeof( void* ) );

    OOE_CHECK( "u8 is 8 bits", sizeof( u8 ) == 1 );
    OOE_CHECK( "u16 is 16 bits", sizeof( u16 ) == 2 );
    OOE_CHECK( "u32 is 32 bits", sizeof( u32 ) == 4 );
    OOE_CHECK( "u64 is 64 bits", sizeof( u64 ) == 8 );
    OOE_CHECK( "up_t is pointer sized", sizeof( up_t ) == sizeof( void* ) );

    OOE_CHECK( "f32 is 32 bits", sizeof( f32 ) == 4 );
    OOE_CHECK( "f64 is 64 bits", sizeof( f64 ) == 8 );

    OOE_CHECK( "c8 is 8 bits", sizeof( c8 ) == 1 );
    OOE_CHECK( "c32 is 32 bits", sizeof( c32 ) == 4 );
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
