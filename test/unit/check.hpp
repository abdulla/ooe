/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_TEST_UNIT_CHECK_HPP
#define OOE_TEST_UNIT_CHECK_HPP

#include "foundation/utility/macro.hpp"

#define check( reason, boolean )\
	if ( !( boolean ) )\
	{\
		ooe::unit::fail();\
		std::cerr <<\
			reason "\n"\
			"\tFile \"" __FILE__ "\", Line " << __LINE__ << ":\n"\
			"\t" #boolean "\n";\
	}

namespace ooe
{
	namespace unit
	{
		void fail( void ) OOE_VISIBLE;
	}
}

#endif	// OOE_TEST_UNIT_CHECK_HPP
