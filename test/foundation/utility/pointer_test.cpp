/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/utility/pointer.hpp"
#include "test/unit/assert.hpp"
#include "test/unit/group.hpp"

namespace
{
	using namespace ooe;

	class data
	{
	protected:
		class destruct_test;
	};

	class data::destruct_test
	{
	public:
		destruct_test( bool& state_ )
			: state( state_ )
		{
		}

		~destruct_test( void )
		{
			state = true;
		}

	private:
		bool& state;
	};

	typedef unit::group< empty_t, data, 2 > group_type;
	typedef group_type::fixture_type fixture_type;
	group_type group( "pointer" );
}

namespace ooe
{
	namespace unit
	{
		template<>
		template<>
			void fixture_type::test< 0 >( void )
		{
			std::cerr << "scoped_ptr\n";

			bool did_destruct = false;
			{
				scoped_ptr< destruct_test > ptr( new destruct_test( did_destruct ) );
			}
			assert( "destruct_test::~destruct_test()", did_destruct );

			up_t* value = new up_t( 7 );
			scoped_ptr< up_t > ptr( value );
			assert( "*ptr == *value", *ptr == *value );
			assert( "ptr == value", ptr == value );
			assert( "ptr.get() == value", ptr.get() == value );
			assert( "ptr.operator ->() == value", ptr.operator ->() == value );
			assert( "ptr.as< up_t >() == value", ptr.as< up_t >() == value );

			scoped_ptr< up_t > other( value );
			assert( "other.release() == value", other.release() == value );

			ptr.swap( other );
			assert( "ptr.get() == 0", ptr.get() == 0 );
			assert( "other.get() == value", other.get() == value );

			other.swap( ptr );
			assert( "ptr.get() == value", ptr.get() == value );
			assert( "other.get() == 0", other.get() == 0 );
		}

		template<>
		template<>
			void fixture_type::test< 1 >( void )
		{
			std::cerr << "shared_ptr\n";

			bool did_destruct = false;
			{
				shared_ptr< destruct_test > ptr( new destruct_test( did_destruct ) );
			}
			assert( "destruct_test::~destruct_test()", did_destruct );

			up_t* value = new up_t( 7 );
			shared_ptr< up_t > ptr( value );
			assert( "*ptr == *value", *ptr == *value );
			assert( "ptr == value", ptr == value );
			assert( "ptr.get() == value", ptr.get() == value );
			assert( "ptr.operator ->() == value", ptr.operator ->() == value );
			assert( "ptr.as< up_t >() == value", ptr.as< up_t >() == value );

			shared_ptr< up_t > other( 0 );
			ptr.swap( other );
			assert( "ptr.get() == 0", ptr.get() == 0 );
			assert( "other.get() == value", other.get() == value );

			other.swap( ptr );
			assert( "ptr.get() == value", ptr.get() == value );
			assert( "other.get() == 0", other.get() == 0 );

			{
				shared_ptr< up_t > copy( ptr );
				assert( "copy.get() == value", copy.get() == value );
			}
			assert( "copy: ptr.get() == value", ptr.get() == value );
		}
	}
}
