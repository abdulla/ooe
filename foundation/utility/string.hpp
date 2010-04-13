/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_UTILITY_STRING_HPP
#define OOE_FOUNDATION_UTILITY_STRING_HPP

#include <string>

#include "foundation/utility/traits.hpp"

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

#endif	// OOE_FOUNDATION_UTILITY_STRING_HPP
