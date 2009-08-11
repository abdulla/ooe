/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_TEST_UNIT_GROUP_HPP
#define OOE_TEST_UNIT_GROUP_HPP

#include <vector>

#include "foundation/utility/tuple.hpp"
#include "test/unit/runner.hpp"

namespace ooe
{
	namespace unit
	{
		class group_base;

		template< typename, up_t >
			struct group;

		template< typename >
			struct fixture;

		template< typename, up_t >
			struct insert_test;

		template< typename type >
			struct insert_test< type, 0 >;

		template< typename type, void ( type::* )( void ) >
			void invoke_test( void );
	}

//--- unit::group_base ---------------------------------------------------------
	class unit::group_base
	{
	public:
		typedef void ( * function_type )( void );
		typedef std::vector< function_type > vector_type;
		typedef vector_type::const_iterator iterator_type;

		iterator_type begin( void ) const;
		iterator_type end( void ) const;
		void push_back( function_type );

	private:
		vector_type vector;
	};

//--- unit::group --------------------------------------------------------------
	template< typename data, up_t size >
		struct unit::group
		: public group_base
	{
		typedef fixture< data > fixture_type;

		group( const std::string& name, runner& runner = global_runner )
			: group_base()
		{
			insert_test< fixture_type, size >::call( *this );
			runner.insert( name, *this );
		}
	};

//--- unit::fixture ------------------------------------------------------------
	template< typename data >
		struct unit::fixture
		: private data
	{
		template< up_t >
			void test( void )
		{
		}
	};

//--- unit::insert_test --------------------------------------------------------
	template< typename type, up_t i >
		struct unit::insert_test
	{
		static void call( group_base& group )
		{
			insert_test< type, i - 1 >::call( group );
			group.push_back( invoke_test< type, &type::template test< i - 1 > > );
		}
	};

	template< typename type >
		struct unit::insert_test< type, 0 >
	{
		static void call( group_base& )
		{
		}
	};

//--- unit::invoke_test --------------------------------------------------------
	template< typename type, void ( type::* member )( void ) >
		void unit::invoke_test( void )
	{
		type instance;
		( instance.*member )();
	}
}

#endif	// OOE_TEST_UNIT_GROUP_HPP
