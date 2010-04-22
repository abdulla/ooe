/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_UTILITY_ARITHMETIC_HPP
#define OOE_FOUNDATION_UTILITY_ARITHMETIC_HPP

#include <algorithm>

#include "foundation/utility/fundamental.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- divide ---------------------------------------------------------------------------------------
inline f32 divide( f32 dividend, f32 divisor )
{
	return dividend / divisor;
}

//--- clamp ----------------------------------------------------------------------------------------
template< typename type >
	type clamp( type value, type min, type max )
{
	return std::max( min, std::min( value, max ) );
}

//--- ceiling --------------------------------------------------------------------------------------
template< typename type >
	type ceiling( type dividend, type divisor )
{
	return dividend / divisor + ( dividend % divisor ? 1 : 0 );
}

//--- floor ----------------------------------------------------------------------------------------
template< typename type >
	type floor( type dividend, type divisor )
{
	return dividend / divisor;
}

//--- round_up -------------------------------------------------------------------------------------
template< typename type >
	type round_up( type dividend, type divisor )
{
	return ceiling( dividend, divisor ) * divisor;
}

//--- round_down -----------------------------------------------------------------------------------
template< typename type >
	type round_down( type dividend, type divisor )
{
	return floor( dividend, divisor ) * divisor;
}

//--- isqrt ----------------------------------------------------------------------------------------
template< typename type >
	typename enable_if< is_unsigned< type > >::type isqrt( type value )
{
	type remainder = 0;
	type root = 0;

	for ( u8 i = 0, end = sizeof( type ) * 4; i != end; ++i )
	{
		root <<= 1;
		remainder = ( remainder << 2 ) + ( value >> 30 );
		value <<= 2;

		if ( ++root > remainder )
			--root;
		else
			remainder -= root++;
	}

	return root >> 1;
}

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_FOUNDATION_UTILITY_ARITHMETIC_HPP
