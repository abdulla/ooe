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

    OOE_CHECK( is_signed< s8 >::value ) << "s8 is not signed";
    OOE_CHECK( is_signed< s16 >::value ) << "s16 is not signed";
    OOE_CHECK( is_signed< s32 >::value ) << "s32 is not signed";
    OOE_CHECK( is_signed< s64 >::value ) << "s64 is not signed";
    OOE_CHECK( is_signed< sp_t >::value ) << "sp_t is not signed";

    OOE_CHECK( is_unsigned< u8 >::value ) << "u8 is not unsigned";
    OOE_CHECK( is_unsigned< u16 >::value ) << "u16 is not unsigned";
    OOE_CHECK( is_unsigned< u32 >::value ) << "u32 is not unsigned";
    OOE_CHECK( is_unsigned< u64 >::value ) << "u64 is not unsigned";
    OOE_CHECK( is_unsigned< up_t >::value ) << "up_t is not unsigned";

    OOE_CHECK( is_floating_point< f32 >::value ) << "f32 is not floating point";
    OOE_CHECK( is_floating_point< f64 >::value ) << "f64 is not floating point";

    OOE_CHECK( is_integral< c8 >::value ) << "c8 is not integral";
    OOE_CHECK( is_integral< c32 >::value ) << "c32 is not integral";
}

OOE_TEST( 1 )
{
    std::cerr << "types have correct size";

    OOE_CHECK( sizeof( s8 ) == 1 ) << "s8 is not 8 bits";
    OOE_CHECK( sizeof( s16 ) == 2 ) << "s16 is not 16 bits";
    OOE_CHECK( sizeof( s32 ) == 4 ) << "s32 is not 32 bits";
    OOE_CHECK( sizeof( s64 ) == 8 ) << "s64 is not 64 bits";
    OOE_CHECK( sizeof( sp_t ) == sizeof( void* ) ) << "sp_t is not pointer sized";

    OOE_CHECK( sizeof( u8 ) == 1 ) << "u8 is not 8 bits";
    OOE_CHECK( sizeof( u16 ) == 2 ) << "u16 is not 16 bits";
    OOE_CHECK( sizeof( u32 ) == 4 ) << "u32 is not 32 bits";
    OOE_CHECK( sizeof( u64 ) == 8 ) << "u64 is not 64 bits";
    OOE_CHECK( sizeof( up_t ) == sizeof( void* ) ) << "up_t is not pointer sized";

    OOE_CHECK( sizeof( f32 ) == 4 ) << "f32 is not 32 bits";
    OOE_CHECK( sizeof( f64 ) == 8 ) << "f64 is not 64 bits";

    OOE_CHECK( sizeof( c8 ) == 1 ) << "c8 is not 8 bits";
    OOE_CHECK( sizeof( c32 ) == 4 ) << "c32 is not 32 bits";
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
