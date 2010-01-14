/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_TEST_UNIT_GROUP_HPP
#define OOE_TEST_UNIT_GROUP_HPP

#include <vector>

#include "foundation/utility/miscellany.hpp"
#include "foundation/utility/pointer.hpp"
#include "foundation/utility/tuple.hpp"
#include "test/unit/runner.hpp"

namespace ooe
{
	namespace unit
	{
		class group_base;

		template< typename, typename, up_t >
			class group;

		template< typename, typename >
			struct fixture;

		template< typename, up_t >
			struct insert_test;

		template< typename group >
			struct insert_test< group, 0 >;

		template< typename group, void ( group::fixture_type::* )( typename group::setup_type& ) >
			void invoke_test( void* );
	}

//--- unit::group_base ---------------------------------------------------------
	class unit::group_base
	{
	public:
		typedef void ( * function_type )( void* );
		typedef std::vector< function_type > vector_type;
		typedef vector_type::const_iterator iterator_type;
		typedef tuple< void*, void ( * )( const void* ) > setup_tuple;

		virtual ~group_base( void ) {}
		virtual setup_tuple create_setup( void ) = 0;

		iterator_type begin( void ) const;
		iterator_type end( void ) const;
		void insert( function_type ) OOE_VISIBLE;

	private:
		vector_type vector;
	};

//--- unit::group --------------------------------------------------------------
	template< typename setup, typename data, up_t size >
		class unit::group
		: public group_base
	{
	public:
		typedef setup setup_type;
		typedef fixture< setup_type, data > fixture_type;

		group( const std::string& name, runner& runner = global_runner )
			: group_base()
		{
			insert_test< group, size >::call( *this );
			runner.insert( name, *this );
		}

	private:
		virtual setup_tuple create_setup( void )
		{
			return setup_tuple( new setup_type, destroy< setup_type > );
		}
	};

//--- unit::fixture ------------------------------------------------------------
	template< typename setup, typename data >
		struct unit::fixture
		: private data
	{
		template< up_t >
			void test( setup& )
		{
		}
	};

//--- unit::insert_test --------------------------------------------------------
	template< typename group, up_t i >
		struct unit::insert_test
	{
		static void call( group_base& base )
		{
			insert_test< group, i - 1 >::call( base );
			base.insert( invoke_test< group, &group::fixture_type::template test< i - 1 > > );
		}
	};

	template< typename group >
		struct unit::insert_test< group, 0 >
	{
		static void call( group_base& )
		{
		}
	};

//--- unit::invoke_test --------------------------------------------------------
	template< typename group,
		void ( group::fixture_type::* member )( typename group::setup_type& ) >
		void unit::invoke_test( void* pointer )
	{
		typename group::fixture_type instance;
		( instance.*member )( *static_cast< typename group::setup_type* >( pointer ) );
	}
}

#endif	// OOE_TEST_UNIT_GROUP_HPP
