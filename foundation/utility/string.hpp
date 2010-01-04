/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_FOUNDATION_UTILITY_STRING_HPP
	#define OOE_FOUNDATION_UTILITY_STRING_HPP

#include <string>

#include "foundation/utility/fundamental.hpp"
#include "foundation/utility/tuple.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- operator << ----------------------------------------------------------------------------------
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

//--- is_stdstring ---------------------------------------------------------------------------------
template< typename type >
	struct is_stdstring
	: public is_like< type, std::string >
{
};

//--- is_cstring -----------------------------------------------------------------------------------
template< typename type >
	struct is_cstring
{
	static const bool value = is_like< type, c8* >::value || is_like< type, const c8* >::value;
};

//--- is_string ------------------------------------------------------------------------------------
template< typename type >
	struct is_string
{
	static const bool value = is_stdstring< type >::value || is_cstring< type >::value;
};

//--- string_size ----------------------------------------------------------------------------------
inline up_t string_size( const std::string& string )
{
	return string.size();
}

inline up_t string_size( const c8* string )
{
	return std::char_traits< c8 >::length( string );
}

//--- string_data ----------------------------------------------------------------------------------
inline const c8* string_data( const std::string& string )
{
	return string.c_str();
}

inline const c8* string_data( const c8* string )
{
	return string;
}

//--- string_make ----------------------------------------------------------------------------------
template< typename t >
	typename no_ref< t >::type string_make( const c8* data, up_t size,
	typename enable_if< is_stdstring< t > >::type* = 0 )
{
	return std::string( data, size );
}

template< typename t >
	typename no_ref< t >::type string_make( const c8* data, up_t,
	typename enable_if< is_cstring< t > >::type* = 0 )
{
	return data;
}

OOE_NAMESPACE_END( ( ooe ) )

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
OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- tuple_less -----------------------------------------------------------------------------------
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

OOE_NAMESPACE_END( ( ooe ) )
#endif

	#undef COMMA
	#undef MINUS
	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
