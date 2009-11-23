/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_COMPONENT_REGISTRY_BUILDER_HPP
	#define OOE_COMPONENT_REGISTRY_BUILDER_HPP

#include "component/registry/local.hpp"
#include "component/registry/module.hpp"

namespace ooe
{
	template< BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT( OOE_PP_LIMIT, typename t, no_t ) >
		class builder;
}


	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "component/registry/builder.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	#endif	// OOE_COMPONENT_REGISTRY_BUILDER_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()
	#define BUILDER_INSERT( z, n, d ) a ## n ->insert< d >( i );
	#define MODULE_INSERT( z, n, d ) module.insert( typeid( t ## n ).name(), a ## n );
	#define POINTER_NEW( z, n, _ ) a ## n( new t ## n )
	#define POINTER_DECLARE( z, n, _ ) scoped_ptr< t ## n > a ## n;

namespace ooe
{
//--- builder ------------------------------------------------------------------
	template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
#if LIMIT == OOE_PP_LIMIT
		class builder
#else
		class builder< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >
#endif
	{
	public:
		builder( ooe::module& module_ )
			: module( module_ ), local( new facade::local )
			BOOST_PP_ENUM_TRAILING( LIMIT, POINTER_NEW, ~ )
		{
		}

		~builder( void )
		{
			module.insert( typeid( facade::local ).name(), local );
			BOOST_PP_REPEAT( LIMIT, MODULE_INSERT, ~ )
		}

		template< typename type >
			void insert( const std::string& name, type function,
			typename enable_if< is_function_pointer< type > >::type* = 0 )
		{
			typedef typename remove_pointer< type >::type function_type;
			up_t i = module.insert( name, typeid( function_type ).name() );
			local->insert( i, function );
			BOOST_PP_REPEAT( LIMIT, BUILDER_INSERT, function_type )
		}

		template< typename type >
			void insert( const std::string& name, type member,
			typename enable_if< is_member_function_pointer< type > >::type* = 0 )
		{
			typedef typename function_of< type >::type signature_type;
			up_t i = module.insert( name, typeid( signature_type ).name() );
			local->insert( i, member );
			BOOST_PP_REPEAT( LIMIT, BUILDER_INSERT, signature_type )
		}

	private:
		ooe::module& module;
		scoped_ptr< facade::local > local;
		BOOST_PP_REPEAT( LIMIT, POINTER_DECLARE, ~ )
	};
}

	#undef POINTER_DECLARE
	#undef POINTER_NEW
	#undef MODULE_INSERT
	#undef BUILDER_INSERT
	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
