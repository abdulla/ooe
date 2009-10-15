/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <paths.h>

#include "foundation/io/file.hpp"
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
			: pair( make_pair() )
		{
			u32 value = 0xdeadbeef;
			descriptor desc( _PATH_TMP "test-file", descriptor::read_write | descriptor::truncate );
			file( desc ).write( &value, sizeof( value ) );

			socket = &pair._1;
			pair._0.send( desc );
		}

	private:
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

			u32 value;
			file file( ::socket->receive() );
			file.seek( 0, file::begin );
			file.read( &value, sizeof( value ) );

			assert( "value == 0xdeadbeef", value == 0xdeadbeef );
		}
	}
}
