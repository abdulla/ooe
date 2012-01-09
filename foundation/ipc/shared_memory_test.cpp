/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/environment.hpp"
#include "foundation/ipc/shared_memory.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

typedef unit::group< anonymous_t, anonymous_t, 1 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "shared memory" );

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

OOE_TEST void fixture_type::test< 0 >( anonymous_t& )
{
    std::cerr << "mapping the same shared memory region twice";

    const char path[] = "/ooe.test.shared-memory";
    ipc::shared_memory memory_1( path, ipc::shared_memory::create, executable::static_page_size );
    ipc::shared_memory memory_2( path );

    OOE_CHECK( "memory_1 != memory_2", memory_1.get() != memory_2.get() );
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
