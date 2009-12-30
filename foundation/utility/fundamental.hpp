/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_UTILITY_FUNDAMENTAL_HPP
#define OOE_FOUNDATION_UTILITY_FUNDAMENTAL_HPP

#include <boost/mpl/find_if.hpp>
#include <boost/mpl/pop_back.hpp>
#include <boost/mpl/reverse.hpp>
#include <boost/mpl/sizeof.hpp>
#include <boost/mpl/vector.hpp>

#include "foundation/utility/namespace.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

typedef boost::mpl::vector< signed char, signed short, signed int, signed long,
	signed long long > signed_types;
typedef boost::mpl::vector< unsigned char, unsigned short, unsigned int, unsigned long,
	unsigned long long > unsigned_types;
typedef boost::mpl::vector< float, double, long double > float_types;
typedef boost::mpl::vector< char, wchar_t > char_types;

typedef boost::mpl::reverse< boost::mpl::pop_back< signed_types >::type >::type signed_ptr;
typedef boost::mpl::reverse< boost::mpl::pop_back< unsigned_types >::type >::type unsigned_ptr;

//--- find_size ------------------------------------------------------------------------------------
template< typename sequence, unsigned size >
	struct find_size
{
	typedef boost::mpl::placeholders::_ _;
	typedef boost::is_same< boost::mpl::sizeof_< _ >, boost::mpl::size_t< size > > predicate;
	typedef typename boost::mpl::find_if< sequence, predicate >::type iterator;
	typedef typename boost::mpl::deref< iterator >::type type;
};

//--- fundamental ----------------------------------------------------------------------------------
typedef find_size< signed_types, 1 >::type s8;
typedef find_size< signed_types, 2 >::type s16;
typedef find_size< signed_types, 4 >::type s32;
typedef find_size< signed_types, 8 >::type s64;
typedef find_size< signed_ptr, sizeof( void* ) >::type sp_t;

typedef find_size< unsigned_types, 1 >::type u8;
typedef find_size< unsigned_types, 2 >::type u16;
typedef find_size< unsigned_types, 4 >::type u32;
typedef find_size< unsigned_types, 8 >::type u64;
typedef find_size< unsigned_ptr, sizeof( void* ) >::type up_t;

typedef find_size< float_types, 4 >::type f32;
typedef find_size< float_types, 8 >::type f64;

typedef find_size< char_types, 1 >::type c8;
typedef find_size< char_types, 4 >::type c32;

struct no_t;

OOE_NAMESPACE_END( ( ooe ) )

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

struct empty_t {};

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

#endif	// OOE_FOUNDATION_UTILITY_FUNDAMENTAL_HPP
