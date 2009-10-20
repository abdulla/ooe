/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <paths.h>

#include "foundation/io/file.hpp"
#include "foundation/io/poll.hpp"
#include "foundation/io/socket.hpp"
#include "foundation/parallel/task.hpp"
#include "test/unit/assert.hpp"
#include "test/unit/group.hpp"

namespace
{
	using namespace ooe;
	ooe::socket* socket;

	class setup
	{
	protected:
		setup( void )
			: pair( make_pair() ), poll( make_pair() )
		{
			u32 value = 0xdeadbeef;
			descriptor desc( _PATH_TMP "test-file", descriptor::read_write | descriptor::truncate );
			file( desc ).write( &value, sizeof( value ) );

			socket = &pair._1;
			pair._0.send( desc );
			pair._0.send( poll._1.desc() );
		}

	private:
		socket_pair pair;
		socket_pair poll;
	};

	typedef unit::group< setup, empty_t, 2 > group_type;
	typedef group_type::fixture_type fixture_type;
	group_type group( "socket" );
}

namespace ooe
{
	namespace unit
	{
		template<>
		template<>
			void fixture_type::test< 0 >( void )
		{
			std::cerr << "send/receive descriptor\n";

			u32 value;
			file file( ::socket->receive() );
			file.seek( 0, file::begin );
			file.read( &value, sizeof( value ) );

			assert( "value == 0xdeadbeef", value == 0xdeadbeef );
		}

		template<>
		template<>
			void fixture_type::test< 1 >( void )
		{
			std::cerr << "poll on shutdown\n";

			socket poll_socket( ::socket->receive() );
			unique_task< void ( socket::shutdown_type ) >
				task( make_function( poll_socket, &socket::shutdown ), socket::read );

			poll poll;
			poll.insert( poll_socket.desc() );
			poll.wait();
		}
	}
}
