/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_TEST_UNIT_CHECK_HPP
#define OOE_TEST_UNIT_CHECK_HPP

#include <boost/preprocessor/cat.hpp>

#define OOE_CHECK( boolean )\
    ( boolean ) ? ( void ) 0 : ooe::unit::fail(), std::cerr <<\
        "\n\n" << #boolean << "\n"\
        "\tLine " << __LINE__ << ", File \"" __FILE__ "\"\n"\
        "\t"

#define OOE_EXCEPT( exception, raise )\
    bool BOOST_PP_CAT( success, __LINE__ ) = false;\
    {\
        try\
        {\
            raise;\
        }\
        catch ( exception& error )\
        {\
            BOOST_PP_CAT( success, __LINE__ ) = true;\
        }\
        catch ( ... )\
        {\
        }\
    }\
    OOE_CHECK( BOOST_PP_CAT( success, __LINE__ ) )

#endif  // OOE_TEST_UNIT_CHECK_HPP
