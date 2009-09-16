/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_FOUNDATION_UTILITY_SCOPED_HPP
	#define OOE_FOUNDATION_UTILITY_SCOPED_HPP

#include "foundation/utility/partial.hpp"

namespace ooe
{
	template< typename >
		class scoped;
}

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "foundation/utility/scoped.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	#endif	// OOE_FOUNDATION_UTILITY_SCOPED_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()

namespace ooe
{
//--- scoped -------------------------------------------------------------------
	template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		class scoped< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
	public:
		typedef ooe::partial< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) > partial_type;
		typedef typename partial_type::function_type function_type;

		scoped( void )
			: partial( function_type()
			BOOST_PP_ENUM_TRAILING_BINARY_PARAMS( LIMIT, t, () BOOST_PP_INTERCEPT ) )
		{
		}

		scoped( function_type function BOOST_PP_ENUM_TRAILING_BINARY_PARAMS
			( LIMIT, typename call_traits< t, >::param_type a ) )
			: partial( function BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) )
		{
		}

		~scoped( void )
		{
			partial();
		}

		void assign( function_type function BOOST_PP_ENUM_TRAILING_BINARY_PARAMS
			( LIMIT, typename call_traits< t, >::param_type a ) )
		{
			partial = partial_type( function BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );
		}

		void clear( void )
		{
			partial.clear();
		}

		r execute( void ) const
		{
			return partial();
		}

	private:
		partial_type partial;
	};
}

	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
