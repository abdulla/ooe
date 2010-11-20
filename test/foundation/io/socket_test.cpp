/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <paths.h>

#include "foundation/io/file.hpp"
#include "foundation/io/poll.hpp"
#include "foundation/io/socket.hpp"
#include "foundation/parallel/thread.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

const c8 path[] = _PATH_TMP "ooe.test.file";

//--- setup ----------------------------------------------------------------------------------------
class setup
{
public:
    setup( void )
        : pair( make_pair() ), poll( make_pair() )
    {
        u32 value = 0xdeadbeef;
        descriptor desc( path, descriptor::read_write | descriptor::truncate );
        file( desc ).write( &value, sizeof( value ) );
        pair._0.send( desc );
    }

    ~setup( void )
    {
        unlink( path );
    }

    descriptor receive( void )
    {
        return pair._1.receive();
    }

    socket get( void ) const
    {
        return poll._1;
    }

    void* shutdown( void* )
    {
        poll._1.shutdown( socket::read );
        return 0;
    }

private:
    socket_pair pair;
    socket_pair poll;
};

typedef unit::group< setup, anonymous_t, 3 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "socket" );

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

template<>
template<>
    void fixture_type::test< 0 >( setup& setup )
{
    std::cerr << "send/receive descriptor\n";

    u32 value;
    file file( setup.receive() );
    file.seek( 0, file::begin );
    file.read( &value, sizeof( value ) );

    OOE_CHECK( "value == 0xdeadbeef", value == 0xdeadbeef );
}

template<>
template<>
    void fixture_type::test< 1 >( setup& setup )
{
    std::cerr << "poll on shutdown\n";

    socket poll_socket( setup.get() );
    thread thread( make_function( setup, &setup::shutdown ), 0 );

    poll poll;
    poll.insert( poll_socket );
    poll.wait();

    thread.join();
}

template<>
template<>
    void fixture_type::test< 2 >( setup& )
{
    std::cerr << "internet query for localhost\n";

    internet_query query( "localhost", "http" );
    std::cout << "Number of addresses for \"localhost\": " <<
        std::distance( query.begin(), query.end() ) << '\n';
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
