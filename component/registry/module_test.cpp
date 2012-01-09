/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "component/registry/builder.hpp"
#include "component/registry/local.hpp"
#include "component/registry/remote.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

void callback( void )
{
}

typedef unit::group< anonymous_t, anonymous_t, 1 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "module" );

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

OOE_TEST void fixture_type::test< 0 >( anonymous_t& )
{
    std::cerr << "build module";

    module module;
    builder< facade::remote > builder( module );
    builder.insert( "callback", callback );
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
