/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_TEST_UNIT_CHECK_HPP
#define OOE_TEST_UNIT_CHECK_HPP

#define OOE_FAIL( reason, detail )\
do\
{\
    ooe::unit::fail();\
    std::cerr <<\
        "\n\n" << reason << "\n"\
        "\tFile \"" __FILE__ "\", Line " << __LINE__ << ":\n"\
        "\t" detail;\
}\
while ( false )

#define OOE_CHECK( reason, boolean )\
do\
{\
    if ( !( boolean ) )\
        OOE_FAIL( reason, #boolean );\
}\
while ( false )

#define OOE_EXCEPT( reason, exception, raise )\
do\
{\
    try\
    {\
        raise;\
        OOE_FAIL( reason, #raise );\
    }\
    catch ( exception& error )\
    {\
    }\
    catch ( ... )\
    {\
        OOE_FAIL( "Caught unexpected exception", #raise );\
    }\
}\
while ( false )

#endif  // OOE_TEST_UNIT_CHECK_HPP
