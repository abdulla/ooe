/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <paths.h>

#include "foundation/io/poll.hpp"
#include "foundation/io/socket.hpp"
#include "foundation/parallel/thread.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

void disconnect( bool& result, const descriptor&, poll::type type )
{
    result = type == poll::disconnect;
}

void shutdown( void* pointer )
{
    static_cast< socket* >( pointer )->shutdown( socket::read );
}

typedef unit::group< anonymous_t, anonymous_t, 1 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "poll" );

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

OOE_TEST( 0 )
{
    std::cerr << "poll on shutdown";

    socket_pair pair = make_pair();
    bool result;
    poll::function_type function( result, disconnect );

    poll poll;
    poll.insert( pair._0, function );

    thread thread( "shutdown", shutdown, &pair._0 );
    poll.wait();

    OOE_CHECK( result );
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
