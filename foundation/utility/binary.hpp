/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_UTILITY_BINARY_HPP
#define OOE_FOUNDATION_UTILITY_BINARY_HPP

#include "foundation/utility/fundamental.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- is_bit_round ---------------------------------------------------------------------------------
template< typename t >
    bool is_bit_round( t value )
{
    return value > 0 && !( value & ( value - 1 ) );
}

//--- bit_round_up ---------------------------------------------------------------------------------
template< typename t >
    t bit_round_up( t value )
{
    if ( !value )
        return 1;

    --value;

    for ( up_t i = 1; i != sizeof( t ) * 8; i <<= 1 )
        value |= value >> i;

    return ++value;
}

//--- bit_round_down -------------------------------------------------------------------------------
template< typename t >
    t bit_round_down( t value )
{
    return bit_round_up( value ) >> 1;
}

//--- bit_count ------------------------------------------------------------------------------------
inline u32 bit_count( u32 value )
{
    value -= ( value >> 1 ) & 0x55555555;
    value = ( ( value >> 2 ) & 0x33333333 ) + ( value & 0x33333333 );
    value += ( value >> 4 ) & 0x0f0f0f0f;
    return value * 0x01010101 >> 24;
}

//--- bit_slide ------------------------------------------------------------------------------------
template< typename t >
    t bit_slide( t value, s8 slide )
{
    return slide < 0 ? value >> -slide : value << slide;
}

//--- saturated_shift ------------------------------------------------------------------------------
template< typename t >
    t saturated_shift( t value, s8 shift )
{
    s8 bits = sizeof( value ) * 8;
    t mask = t( 1 ) << ( bits - shift );
    return shift >= bits || ( shift && value & mask ) ? ~t( 0 ): value << shift;
}

//--- saturated_slide ------------------------------------------------------------------------------
template< typename t >
    t saturated_slide( t value, s8 slide )
{
    return slide < 0 ? value >> -slide : saturated_shift( value, slide );
}

//--- parity ---------------------------------------------------------------------------------------
inline u32 parity( u32 value )
{
    value ^= value >> 16;
    value ^= value >> 8;
    value ^= value >> 4;
    value &= 0x0f;
    return ( 0x6996 >> value ) & 0x01;
}

//--- little_endian --------------------------------------------------------------------------------
template< typename t >
    t little_endian( t value )
{
    u8* byte = reinterpret_cast< u8* >( &value );
    t result = 0;

    for ( u8 i = 0; i != sizeof( t ); ++i )
        result |= byte[ i ] << ( i * 8 );

    return result;
}

//--- big_endian -----------------------------------------------------------------------------------
template< typename t >
    t big_endian( t value )
{
    u8* byte = reinterpret_cast< u8* >( &value );
    t result = 0;

    for ( u8 i = 0; i != sizeof( t ); ++i )
        result |= byte[ sizeof( t ) - i - 1 ] << ( i * 8 );

    return result;
}

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_UTILITY_BINARY_HPP
