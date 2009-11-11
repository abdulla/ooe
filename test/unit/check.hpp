/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_TEST_UNIT_CHECK_HPP
#define OOE_TEST_UNIT_CHECK_HPP

#define check( reason, boolean )\
	if ( !( boolean ) )\
	{\
		ooe::unit::status = false;\
		std::cerr <<\
			reason "\n"\
			"\tFile \"" __FILE__ "\", Line " << __LINE__ << ":\n"\
			"\t" #boolean "\n";\
	}

#endif	// OOE_TEST_UNIT_CHECK_HPP
