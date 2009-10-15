/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <paths.h>

#include "foundation/io/memory.hpp"
#include "foundation/io/socket.hpp"
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
			: desc( _PATH_TMP "test-file", descriptor::read_write | descriptor::truncate ),
			pair( make_pair() )
		{
			desc.resize( sizeof( u32 ) );
			memory memory( desc, memory::write );
			*memory.as< u32 >() = 0xdeadbeef;

			socket = &pair._1;
			pair._0.send( desc );
		}

	private:
		descriptor desc;
		socket_pair pair;
	};

	typedef unit::group< setup, empty_t, 1 > group_type;
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

			descriptor desc = ::socket->receive();
			memory memory( desc );

			assert( "descriptor == 0xdeadbeef", *memory.as< u32 >() == 0xdeadbeef );
		}
	}
}
