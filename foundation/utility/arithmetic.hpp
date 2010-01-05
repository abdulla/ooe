/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_UTILITY_ARITHMETIC_HPP
#define OOE_FOUNDATION_UTILITY_ARITHMETIC_HPP

#include <algorithm>

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
template< std::size_t alignment >
	std::size_t round_up( std::size_t size )
{
	std::size_t quotient = size / alignment;
	return ( quotient + 1 ) * alignment;
}

//--- round_down -----------------------------------------------------------------------------------
template< std::size_t alignment >
	std::size_t round_down( std::size_t size )
{
	std::size_t quotient = size / alignment;
	return quotient * alignment;
}

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_FOUNDATION_UTILITY_ARITHMETIC_HPP
