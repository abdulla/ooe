/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_TEST_UNIT_CHECK_HPP
#define OOE_TEST_UNIT_CHECK_HPP

#include "foundation/utility/macro.hpp"

#define check( reason, boolean )\
do\
{\
	if ( !( boolean ) )\
	{\
		ooe::unit::fail();\
		std::cerr <<\
			reason "\n"\
			"\tFile \"" __FILE__ "\", Line " << __LINE__ << ":\n"\
			"\t" #boolean "\n";\
	}\
}\
while ( false )

#define except( reason, raise )\
do\
{\
	try\
	{\
		raise;\
		ooe::unit::fail();\
		std::cerr <<\
			reason "\n"\
			"\tFile \"" __FILE__ "\", Line " << __LINE__ << ":\n"\
			"\t" #raise "\n";\
	}\
	catch ( ... )\
	{\
	}\
}\
while ( false )

#endif	// OOE_TEST_UNIT_CHECK_HPP
