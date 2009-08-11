/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_TEST_UNIT_ASSERT_HPP
#define OOE_TEST_UNIT_ASSERT_HPP

namespace ooe
{
	namespace unit
	{
		void assert( const c8*, bool );
		void assert( const std::string&, bool );

		void expect( const c8*, bool );
		void expect( const std::string&, bool );

		void fail( const c8* );
		void fail( const std::string& );
	}
}

#endif	// OOE_TEST_UNIT_ASSERT_HPP
