/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_FOUNDATION_UTILITY_TRAITS_HPP
	#define OOE_FOUNDATION_UTILITY_TRAITS_HPP

#if __GNUC__ >= 4 && __GNUC_MINOR__ >= 3
	#include <tr1/type_traits>
	#include <boost/type_traits/function_traits.hpp>

	namespace traits = std::tr1;
#else
	#include <boost/type_traits.hpp>

	namespace traits = boost;
#endif

#include <boost/call_traits.hpp>
#include <boost/static_assert.hpp>
#include <boost/utility/enable_if.hpp>

#include "foundation/utility/preprocessor.hpp"

namespace std
{
	template< typename >
		class allocator;

	template< typename, typename >
		class vector;

	template< typename, typename >
		class list;

	template< typename, typename >
		class deque;
}

namespace ooe
{
	using traits::add_const;
	using traits::add_pointer;
	using traits::add_reference;
	using traits::extent;
	using traits::false_type;
	using traits::has_trivial_copy;
	using traits::is_arithmetic;
	using traits::is_array;
	using traits::is_class;
	using traits::is_const;
	using traits::is_enum;
	using traits::is_function;
	using traits::is_fundamental;
	using traits::is_member_function_pointer;
	using traits::is_member_object_pointer;
	using traits::is_member_pointer;
	using traits::is_pod;
	using traits::is_pointer;
	using traits::is_reference;
	using traits::is_same;
	using traits::is_union;
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

//--- no_ptr -------------------------------------------------------------------
	template< typename t >
		struct no_ptr
		: public remove_cv< typename remove_pointer< t >::type >
	{
	};

//--- no_ref -------------------------------------------------------------------
	template< typename t >
		struct no_ref
		: public remove_cv< typename remove_reference< t >::type >
	{
	};

//--- no_qual ------------------------------------------------------------------
	template< typename t >
		struct no_qual
		: public no_ptr< typename no_ref< t >::type >
	{
	};

//--- remove_member ------------------------------------------------------------
	template< typename t >
		struct remove_member
	{
		typedef t type;
	};

	template< typename r, typename t >
		struct remove_member< r t::* >
	{
		typedef r type;
	};

//--- remove_callable ----------------------------------------------------------
	template< typename t >
		struct remove_callable
		: public remove_pointer< typename remove_member< t >::type >
	{
	};

//--- is_empty -----------------------------------------------------------------
	template< typename t >
		struct is_empty
		: public traits::is_empty< typename no_ref< t >::type >
	{
	};

//--- is_like ------------------------------------------------------------------
	template< typename a, typename b >
		struct is_like
		: public is_same< typename no_ref< a >::type, typename no_ref< b >::type >
	{
	};

//--- is_function_pointer ------------------------------------------------------
	template< typename type >
		struct is_function_pointer
		: public is_function< typename remove_pointer< type >::type >
	{
	};

//--- is_callable --------------------------------------------------------------
	template< typename type >
		struct is_callable
		: public is_function< typename remove_callable< type >::type >
	{
	};

//--- callable_traits ----------------------------------------------------------
	template< typename type >
		struct callable_traits
		: public function_traits< typename remove_callable< type >::type >
	{
	};

//--- member_of ----------------------------------------------------------------
	template< typename >
		struct member_of;

	template< typename r, typename t >
		struct member_of< r t::* >
	{
		typedef t type;
	};

//--- is_stdcontainer ----------------------------------------------------------
	template< typename t >
		struct is_stdcontainer
	{
		template< typename >
			struct apply
			: public false_type
		{
		};

		template< typename type, typename allocator >
			struct apply< std::vector< type, allocator > >
			: public true_type
		{
		};

		template< typename type, typename allocator >
			struct apply< std::list< type, allocator > >
			: public true_type
		{
		};

		template< typename type, typename allocator >
			struct apply< std::deque< type, allocator > >
			: public true_type
		{
		};

		static const bool value = apply< typename no_ref< t >::type >::value;
	};
}

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "foundation/utility/traits.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	#endif	// OOE_FOUNDATION_UTILITY_TRAITS_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()

namespace ooe
{
//--- function_of --------------------------------------------------------------
	template< typename >
		struct function_of;

	template< typename t, typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct function_of< r ( t::* )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
		typedef r type( t* BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, t ) );
	};

#if LIMIT
//--- is_template --------------------------------------------------------------
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
}

	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
