/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_TEST_UNIT_ASSERT_HPP
#define OOE_TEST_UNIT_ASSERT_HPP

#include "foundation/utility/macro.hpp"
#include "foundation/utility/string.hpp"

#ifdef assert
#undef assert
#endif

namespace ooe
{
	namespace unit
	{
		void assert( const c8*, bool ) OOE_VISIBLE;
		void assert( const std::string&, bool ) OOE_VISIBLE;

		void expect( const c8*, bool ) OOE_VISIBLE;
		void expect( const std::string&, bool ) OOE_VISIBLE;

		void fail( const c8* ) OOE_VISIBLE;
		void fail( const std::string& ) OOE_VISIBLE;
	}
}

#endif	// OOE_TEST_UNIT_ASSERT_HPP
