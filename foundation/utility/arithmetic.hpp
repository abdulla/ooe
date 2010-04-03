/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_UTILITY_ARITHMETIC_HPP
#define OOE_FOUNDATION_UTILITY_ARITHMETIC_HPP

#include <algorithm>

#include "foundation/utility/fundamental.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- clamp ----------------------------------------------------------------------------------------
template< typename type >
	type clamp( type value, type min, type max )
{
	return std::max( min, std::min( value, max ) );
}

//--- divide ---------------------------------------------------------------------------------------
inline f32 divide( u32 x, u32 y )
{
	return static_cast< f32 >( x ) / static_cast< f32 >( y );
}

//--- round_up -------------------------------------------------------------------------------------
inline up_t round_up( up_t dividend, up_t divisor )
{
	up_t quotient = dividend / divisor + ( dividend % divisor ? 1 : 0 );
	return quotient * divisor;
}

template< up_t divisor >
	up_t round_up( up_t dividend )
{
	return round_up( dividend, divisor );
}

//--- round_down -----------------------------------------------------------------------------------
inline up_t round_down( up_t dividend, up_t divisor )
{
	up_t quotient = dividend / divisor;
	return quotient * divisor;
}

template< up_t divisor >
	up_t round_down( up_t dividend )
{
	return round_down( dividend, divisor );
}

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_FOUNDATION_UTILITY_ARITHMETIC_HPP
