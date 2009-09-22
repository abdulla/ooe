/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_UTILITY_CONVERT_HPP
#define OOE_FOUNDATION_UTILITY_CONVERT_HPP

#include <cstdio>

#include "foundation/utility/fundamental.hpp"

namespace ooe
{
//--- decimal ------------------------------------------------------------------
	template< typename type >
		type& operator <<( type& out, c8 value )
	{
		c8 buffer[ 32 ];
		std::snprintf( buffer, sizeof( buffer ), "%c", value );
		return out << buffer;
	}

	template< typename type >
		type& operator <<( type& out, c32 value )
	{
		c8 buffer[ 32 ];
		std::snprintf( buffer, sizeof( buffer ), "%lc", value );
		return out << buffer;
	}

	template< typename type >
		type& operator <<( type& out, s32 value )
	{
		c8 buffer[ 32 ];
		std::snprintf( buffer, sizeof( buffer ), "%i", value );
		return out << buffer;
	}

	template< typename type >
		type& operator <<( type& out, u32 value )
	{
		c8 buffer[ 32 ];
		std::snprintf( buffer, sizeof( buffer ), "%u", value );
		return out << buffer;
	}

	template< typename type >
		type& operator <<( type& out, sp_t value )
	{
		c8 buffer[ 32 ];
		std::snprintf( buffer, sizeof( buffer ), "%li", value );
		return out << buffer;
	}

	template< typename type >
		type& operator <<( type& out, up_t value )
	{
		c8 buffer[ 32 ];
		std::snprintf( buffer, sizeof( buffer ), "%lu", value );
		return out << buffer;
	}

	template< typename type >
		type& operator <<( type& out, f32 value )
	{
		c8 buffer[ 32 ];
		std::snprintf( buffer, sizeof( buffer ), "%f", value );
		return out << buffer;
	}

	template< typename t0, typename t1 >
		t0& operator <<( t0& out, const t1* value )
	{
		c8 buffer[ 32 ];
		u32 field_width = sizeof( void* ) * 2 + 2;
		up_t pointer = reinterpret_cast< up_t >( value );
		std::snprintf( buffer, sizeof( buffer ), "%#0*lx", field_width, pointer );
		return out << buffer;
	}

	template< typename type >
		type& operator <<( type& out, bool value )
	{
		return out << ( value ? "true" : "false" );
	}

//--- hexadecimal --------------------------------------------------------------
	template< typename type >
		struct hexadecimal
	{
		const type value;

		hexadecimal( type value_ )
			: value( value_ )
		{
		}
	};

	inline hexadecimal< u32 > hex( s32 value )
	{
		return hexadecimal< u32 >( value );
	}

	inline hexadecimal< u32 > hex( u32 value )
	{
		return hexadecimal< u32 >( value );
	}

	inline hexadecimal< u64 > hex( s64 value )
	{
		return hexadecimal< u64 >( value );
	}

	inline hexadecimal< u64 > hex( u64 value )
	{
		return hexadecimal< u64 >( value );
	}

	template< typename type >
		type& operator <<( type& out, hexadecimal< u32 > hex )
	{
		c8 buffer[ 32 ];
		std::snprintf( buffer, sizeof( buffer ), "%x", hex.value );
		return out << buffer;
	}

	template< typename type >
		type& operator <<( type& out, hexadecimal< u64 > hex )
	{
		c8 buffer[ 32 ];
		std::snprintf( buffer, sizeof( buffer ), "%llx", hex.value );
		return out << buffer;
	}
}

#endif	// OOE_FOUNDATION_UTILITY_CONVERT_HPP
