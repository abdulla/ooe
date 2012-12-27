/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

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

#define OOE_CHECK( boolean )\
    ( boolean ) ? ( void ) 0 : ooe::unit::fail(), std::cerr <<\
        "\n\n" << #boolean << "\n"\
        "\tLine " << __LINE__ << ", File \"" __FILE__ "\"\n"\
        "\t"

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
