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
		type& operator <<( type& out, s64 value )
	{
		c8 buffer[ 32 ];
		std::snprintf( buffer, sizeof( buffer ), "%lli", value );
		return out << buffer;
	}

	template< typename type >
		type& operator <<( type& out, u64 value )
	{
		c8 buffer[ 32 ];
		std::snprintf( buffer, sizeof( buffer ), "%llu", value );
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

	/*template< typename type >
		type& operator <<( type& out, bool value )
	{
		return out << ( value ? "true" : "false" );
	}*/

//--- hexadecimal --------------------------------------------------------------
	template< typename type >
		struct hexadecimal
	{
		const type value;

		explicit hexadecimal( type value_ )
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

	inline hexadecimal< up_t > hex( sp_t value )
	{
		return hexadecimal< up_t >( value );
	}

	inline hexadecimal< up_t > hex( up_t value )
	{
		return hexadecimal< up_t >( value );
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

	template< typename type >
		type& operator <<( type& out, hexadecimal< up_t > hex )
	{
		c8 buffer[ 32 ];
		std::snprintf( buffer, sizeof( buffer ), "%lx", hex.value );
		return out << buffer;
	}

//--- pointer ------------------------------------------------------------------
	struct pointer
	{
		const up_t value;

		explicit pointer( const void* value_ )
			: value( reinterpret_cast< up_t >( value_ ) )
		{
		}
	};

	inline pointer ptr( const void* value )
	{
		return pointer( value );
	}

	template< typename type >
		type& operator <<( type& out, pointer ptr )
	{
		c8 buffer[ 32 ];
		u32 field_width = sizeof( void* ) * 2 + 2;
		std::snprintf( buffer, sizeof( buffer ), "%#0*lx", field_width, ptr.value );
		return out << buffer;
	}

//--- repeat -------------------------------------------------------------------
	template< typename t0, typename t1 >
		struct repeat
	{
		const t0 value;
		const t1 number;

		explicit repeat( t0 value_, t1 number_ )
			: value( value_ ), number( number_ )
		{
		}
	};

	template< typename t0, typename t1 >
		repeat< t0, t1 > rep( t0 value, t1 number )
	{
		return repeat< t0, t1 >( value, number );
	}

	template< typename type, typename t0, typename t1 >
		type& operator <<( type& out, const repeat< t0, t1 >& rep )
	{
		for ( t1 i = 0; i != rep.number; ++i )
			out << rep.value;

		return out;
	}
}

#endif	// OOE_FOUNDATION_UTILITY_CONVERT_HPP
