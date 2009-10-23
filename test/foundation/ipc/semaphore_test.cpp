/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifdef __APPLE__
#include <iostream>

#include "foundation/io/socket.hpp"
#include "foundation/ipc/semaphore.hpp"
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
			: pair( make_pair() )
		{
			ipc::semaphore semaphore( "ooe.test.semaphore", ipc::semaphore::create );

			socket = &pair._1;
			pair._0.send( semaphore );
		}

	private:
		socket_pair pair;
	};

	typedef unit::group< setup, empty_t, 1 > group_type;
	typedef group_type::fixture_type fixture_type;
	group_type group( "semaphore" );
}

namespace ooe
{
	namespace unit
	{
		template<>
		template<>
			void fixture_type::test< 0 >( void )
		{
			std::cerr << "send/receive semaphore\n";

			ipc::semaphore semapore( ::socket->receive() );
		}
	}
}
#endif
