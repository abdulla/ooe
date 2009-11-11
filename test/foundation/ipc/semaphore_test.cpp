/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifdef __APPLE__
#include <iostream>

#include "foundation/io/socket.hpp"
#include "foundation/ipc/semaphore.hpp"
#include "test/unit/group.hpp"

namespace
{
	using namespace ooe;

	class setup
	{
	public:
		setup( void )
			: pair( make_pair() )
		{
			ipc::semaphore semaphore( "ooe.test.semaphore", ipc::semaphore::create );
			pair._0.send( semaphore );
		}

		descriptor receive( void )
		{
			return pair._1.receive();
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
			void fixture_type::test< 0 >( setup& setup )
		{
			std::cerr << "send/receive semaphore\n";

			ipc::semaphore semapore( setup.receive() );
		}
	}
}
#endif
