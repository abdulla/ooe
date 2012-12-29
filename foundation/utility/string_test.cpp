/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "foundation/utility/string.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

typedef unit::group< anonymous_t, anonymous_t, 3 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "string" );

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

OOE_TEST( 0 )
{
    std::cerr << "string append\n";

    std::string value;

    value << std::string("a");
    OOE_CHECK( value == "a" );

    value << "b";
    OOE_CHECK( value == "ab" );

    value << 'c';
    OOE_CHECK( value == "abc" );
}

OOE_TEST( 1 )
{
    std::cerr << "string traits\n";

    OOE_CHECK( is_stdstring< std::string >::value );
    OOE_CHECK( !is_stdstring< char* >::value );
    OOE_CHECK( !is_stdstring< up_t >::value );

    OOE_CHECK( !is_cstring< std::string >::value );
    OOE_CHECK( is_cstring< char* >::value );
    OOE_CHECK( !is_cstring< up_t >::value );

    OOE_CHECK( is_string< std::string >::value );
    OOE_CHECK( is_string< char* >::value );
    OOE_CHECK( !is_string< up_t >::value );

}

OOE_TEST( 2 )
{
    std::cerr << "string functions\n";

    std::string value;
    for ( up_t i = 0; i < 16; ++i )
    {
        OOE_CHECK( string_size( value ) == i );
        OOE_CHECK( string_size( value.c_str() ) == i );
        value += 'a';
    }

    value = "across the universe";
    OOE_CHECK( !std::strcmp( string_data( value ), "across the universe" ) );
    OOE_CHECK( !std::strcmp( string_data( value.c_str() ), "across the universe" ) );

    OOE_CHECK( string_make< std::string >( "aaaa", 4 ) == "aaaa" );
    OOE_CHECK( !std::strcmp( string_make< const char* >( "bbb", 3 ), "bbb" ) );
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
