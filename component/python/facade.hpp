/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_COMPONENT_PYTHON_FACADE_HPP
	#define OOE_COMPONENT_PYTHON_FACADE_HPP

#include <vector>

#include "component/python/traits.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( python ) )

template< typename >
	struct invoke_function;

template< typename, typename >
	struct invoke_member;

//--- invoke ---------------------------------------------------------------------------------------
template< typename type >
	struct invoke
{
};

OOE_NAMESPACE_END( ( ooe )( python ) )

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "component/python/facade.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	#endif	// OOE_COMPONENT_PYTHON_FACADE_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()

	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
