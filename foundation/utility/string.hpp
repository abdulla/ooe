/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_FOUNDATION_UTILITY_STRING_HPP
	#define OOE_FOUNDATION_UTILITY_STRING_HPP

#include <string>

#include "foundation/utility/fundamental.hpp"
#include "foundation/utility/tuple.hpp"

namespace ooe
{
	inline std::string& operator <<( std::string& out, const std::string& value )
	{
		return out += value;
	}

	inline std::string& operator <<( std::string& out, const c8* value )
	{
		return out += value;
	}

	inline std::string& operator <<( std::string& out, c8 value )
	{
		return out += value;
	}

	template< typename type >
		struct is_stdstring
		: public is_like< type, std::string >
	{
	};

	template< typename type >
		struct is_cstring
	{
		static const bool value = is_like< type, c8* >::value || is_like< type, const c8* >::value;
	};

	template< typename type >
		struct is_string
	{
		static const bool value = is_stdstring< type >::value || is_cstring< type >::value;
	};

	inline up_t string_size( const std::string& string )
	{
		return string.size();
	}

	inline up_t string_size( const c8* string )
	{
		return std::char_traits< c8 >::length( string );
	}

	inline const c8* string_data( const std::string& string )
	{
		return string.c_str();
	}

	inline const c8* string_data( const c8* string )
	{
		return string;
	}
}

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "foundation/utility/string.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	#endif	// OOE_FOUNDATION_UTILITY_STRING_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()
	#define MINUS BOOST_PP_DEC( LIMIT )
	#define COMMA BOOST_PP_COMMA_IF( MINUS )

#if LIMIT
namespace ooe
{
	template< BOOST_PP_ENUM_PARAMS( MINUS, typename s )
		BOOST_PP_ENUM_TRAILING_PARAMS( MINUS, typename t ) COMMA typename u, typename v >
		struct tuple_less< LIMIT,
		tuple< BOOST_PP_ENUM_PARAMS( MINUS, s ) COMMA u >,
		tuple< BOOST_PP_ENUM_PARAMS( MINUS, t ) COMMA v > >
	{
		typedef tuple< BOOST_PP_ENUM_PARAMS( MINUS, s ) COMMA u > a;
		typedef tuple< BOOST_PP_ENUM_PARAMS( MINUS, t ) COMMA v > b;

		static bool call( const a& x, const b& y,
			typename enable_if< is_same< typename no_ref< u >::type, std::string > >::type* = 0,
			typename enable_if< is_same< typename no_ref< v >::type, std::string > >::type* = 0 )
		{
			s32 result = at< MINUS >( x ).compare( at< MINUS >( y ) );
			return result < 0 || ( !result && tuple_less< MINUS, a, b >::call( x, y ) );
		}
	};
}
#endif

	#undef COMMA
	#undef MINUS
	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
