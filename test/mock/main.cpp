/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/utility/macro.hpp"
#include "foundation/utility/miscellany.hpp"
#include "test/mock/inject.hpp"

void func_a( const c8* string )
{
    std::cout << "func_a: " << string << '\n';
}

void func_b( const c8* string )
{
    std::cout << "func_b: " << string << '\n';
}

//--- main -----------------------------------------------------------------------------------------
extern "C" ooe::s32 main( ooe::s32, ooe::c8** /*, ooe::c8** envp*/ )
{
    ooe::mock::inject( byte_cast< void* >( func_b ), byte_cast< void* >( func_a ) );
    func_b( "this string was passed to func_b" );
    return 0;
}
