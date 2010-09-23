/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_FOUNDATION_UTILITY_TRAITS_HPP
	#define OOE_FOUNDATION_UTILITY_TRAITS_HPP

#if __GNUC__ >= 4 && __GNUC_MINOR__ >= 3
	#include <tr1/type_traits>
	#include <boost/type_traits/function_traits.hpp>

	#define OOE_IS_EMPTY( type )			traits::is_empty< type >
	#define OOE_STATIC_ASSERT( boolean )	BOOST_STATIC_ASSERT( boolean )

	namespace traits = std::tr1;
#else
	#if !defined( BOOST_MSVC_FULL_VER ) && !defined( BOOST_MSVC_FULL_VER_WORKAROUND_GUARD )
		#define BOOST_MSVC_FULL_VER 0
		#define BOOST_MSVC_FULL_VER_WORKAROUND_GUARD 0
	#endif

	#include <boost/type_traits.hpp>

	#define OOE_IS_EMPTY( type )			false_type
	#define OOE_STATIC_ASSERT( boolean )	typedef bool BOOST_JOIN( STATIC_ASSERT, __LINE__ )

	namespace traits = boost;
#endif

#include <boost/call_traits.hpp>
#include <boost/static_assert.hpp>
#include <boost/utility/enable_if.hpp>

#include "foundation/utility/fundamental.hpp"
#include "foundation/utility/preprocessor.hpp"

OOE_NAMESPACE_BEGIN( ( std ) )

template< typename >
	class allocator;

template< typename >
	class less;

template< typename, typename >
	class pair;

template< typename, typename >
	class vector;

template< typename, typename >
	class list;

template< typename, typename >
	class deque;

template< typename, typename, typename >
	class set;

template< typename, typename, typename >
	class multiset;

template< typename, typename, typename, typename >
	class map;

template< typename, typename, typename, typename >
	class multimap;

OOE_NAMESPACE_END( ( std ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

using traits::add_const;
using traits::add_pointer;
using traits::add_reference;
using traits::extent;
using traits::false_type;
using traits::has_trivial_copy;
using traits::is_arithmetic;
using traits::is_class;
using traits::is_const;
using traits::is_enum;
using traits::is_floating_point;
using traits::is_function;
using traits::is_fundamental;
using traits::is_integral;
using traits::is_member_function_pointer;
using traits::is_member_object_pointer;
using traits::is_member_pointer;
using traits::is_pod;
using traits::is_pointer;
using traits::is_reference;
using traits::is_same;
using traits::is_signed;
using traits::is_union;
using traits::is_unsigned;
using traits::remove_cv;
using traits::remove_extent;
using traits::remove_pointer;
using traits::remove_reference;
using traits::true_type;

using boost::call_traits;
using boost::disable_if;
using boost::disable_if_c;
using boost::enable_if;
using boost::enable_if_c;
using boost::function_traits;

//--- no_ptr ---------------------------------------------------------------------------------------
template< typename t >
	struct no_ptr
	: public remove_cv< typename remove_pointer< t >::type >
{
};

//--- no_ref ---------------------------------------------------------------------------------------
template< typename t >
	struct no_ref
	: public remove_cv< typename remove_reference< t >::type >
{
};

//--- no_qual --------------------------------------------------------------------------------------
template< typename t >
	struct no_qual
	: public no_ptr< typename no_ref< t >::type >
{
};

//--- remove_member --------------------------------------------------------------------------------
template< typename t >
	struct remove_member
{
	typedef t type;
};

template< typename t, typename r >
	struct remove_member< r t::* >
{
	typedef r type;
};

//--- remove_callable ------------------------------------------------------------------------------
template< typename t >
	struct remove_callable
	: public remove_pointer< typename remove_member< t >::type >
{
};

//--- is_like --------------------------------------------------------------------------------------
template< typename a, typename b >
	struct is_like
	: public is_same< typename no_ref< a >::type, typename no_ref< b >::type >
{
};

//--- is_empty -------------------------------------------------------------------------------------
template< typename t >
	struct is_empty
	: public OOE_IS_EMPTY( typename no_ref< t >::type )
{
};

//--- is_array -------------------------------------------------------------------------------------
template< typename t >
	struct is_array
	: public traits::is_array< typename no_ref< t >::type >
{
};

//--- is_function_pointer --------------------------------------------------------------------------
template< typename type >
	struct is_function_pointer
	: public is_function< typename remove_pointer< type >::type >
{
};

//--- is_callable ----------------------------------------------------------------------------------
template< typename type >
	struct is_callable
	: public is_function< typename remove_callable< type >::type >
{
};

//--- callable_traits ------------------------------------------------------------------------------
template< typename type >
	struct callable_traits
	: public function_traits< typename remove_callable< type >::type >
{
};

//--- member_of ------------------------------------------------------------------------------------
template< typename >
	struct member_of;

template< typename r, typename t >
	struct member_of< r t::* >
{
	typedef t type;
};

//--- reserve --------------------------------------------------------------------------------------
template< typename t >
	void reserve( t&, up_t )
{
}

template< typename t >
	void reserve( std::vector< t, std::allocator< t > >& vector, up_t size )
{
	vector.reserve( size );
}

//--- container_size -------------------------------------------------------------------------------
template< typename, typename, typename = void >
	struct container_size;

template< typename t, typename v >
	struct container_size< t, v, typename enable_if< is_pod< v > >::type >
{
	template< typename f >
		static up_t call( const t& value, f )
	{
		return value.size() * sizeof( v );
	}
};

template< typename t, typename v >
	struct container_size< t, v, typename disable_if< is_pod< v > >::type >
{
	template< typename f >
		static up_t call( const t& value, f function )
	{
		up_t sum = 0;

		for ( typename t::const_iterator i = value.begin(), end = value.end(); i != end; ++i )
			sum += function( *i );

		return sum;
	}
};

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "foundation/utility/traits.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

//--- is_stdcontainer ------------------------------------------------------------------------------
template< typename t >
	struct is_stdcontainer
{
	static const bool value =
		is_template2< t, std::vector >::value ||
		is_template2< t, std::list >::value ||
		is_template2< t, std::deque >::value ||
		is_template3< t, std::set >::value ||
		is_template3< t, std::multiset >::value ||
		is_template4< t, std::map >::value ||
		is_template4< t, std::multimap >::value;
};

//--- is_sequence ----------------------------------------------------------------------------------
template< typename t >
	struct is_sequence
{
	static const bool value =
		is_template2< t, std::vector >::value ||
		is_template2< t, std::list >::value ||
		is_template2< t, std::deque >::value;
};

//--- is_set ---------------------------------------------------------------------------------------
template< typename t >
	struct is_set
{
	static const bool value =
		is_template3< t, std::set >::value ||
		is_template3< t, std::multiset >::value;
};

//--- is_map ---------------------------------------------------------------------------------------
template< typename t >
	struct is_map
{
	static const bool value =
		is_template4< t, std::map >::value ||
		is_template4< t, std::multimap >::value;
};

//--- is_pair --------------------------------------------------------------------------------------
template< typename t >
	struct is_pair
	: public is_template2< t, std::pair >
{
};

OOE_NAMESPACE_END( ( ooe ) )

	#endif	// OOE_FOUNDATION_UTILITY_TRAITS_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()

//--- remove_member --------------------------------------------------------------------------------
template< typename t, typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
	struct remove_member< r ( t::* )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) const >
{
	typedef r ( type )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
};

//--- function_of ----------------------------------------------------------------------------------
template< typename >
	struct function_of;

template< typename t, typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
	struct function_of< r ( t::* )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
{
	typedef r type( t* BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, t ) );
};

template< typename t, typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
	struct function_of< r ( t::* )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) const >
{
	typedef r type( t* BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, t ) );
};

#if LIMIT
//--- is_template ----------------------------------------------------------------------------------
template< typename a, template< BOOST_PP_ENUM_PARAMS( LIMIT, typename s ) > class b >
	struct BOOST_PP_CAT( is_template, LIMIT )
{
	template< typename >
		struct apply
		: public false_type
	{
	};

	template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
		struct apply< b< BOOST_PP_ENUM_PARAMS( LIMIT, t ) > >
		: public true_type
	{
	};

	static const bool value = apply< typename no_ref< a >::type >::value;
};
#endif

	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
