/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

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

//--- parity ---------------------------------------------------------------------------------------
inline u32 parity( u32 value )
{
	value ^= value >> 16;
	value ^= value >> 8;
	value ^= value >> 4;
	value &= 0x0f;
	return ( 0x6996 >> value ) & 0x01;
}

//--- endian_swap ----------------------------------------------------------------------------------
inline u8 endian_swap( u8 value )
{
	return value;
}

inline u16 endian_swap( u16 value_ )
{
	u32 value = value_;
	return static_cast< u16 >( ( value >> 8 ) | ( value << 8 ) );
}

inline u32 endian_swap( u32 value )
{
	return ( value >> 24 ) | ( value << 24 ) |
		( ( value & 0x00ff0000 ) >> 8 ) | ( ( value & 0x0000ff00 ) << 8 );
}

inline u64 endian_swap( u64 value )
{
	return ( value >> 56 ) |
		( ( value & 0x00ff000000000000ll ) >> 40 ) |
		( ( value & 0x0000ff0000000000ll ) >> 24 ) |
		( ( value & 0x000000ff00000000ll ) >> 8 ) |
		( value << 56 ) |
		( ( value & 0x000000000000ff00ll ) << 40 ) |
		( ( value & 0x0000000000ff0000ll ) << 24 ) |
		( ( value & 0x00000000ff000000ll ) << 8 );
}

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_FOUNDATION_UTILITY_BINARY_HPP
