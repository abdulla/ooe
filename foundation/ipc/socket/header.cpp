/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/utility/error.hpp"

#include "external/nipc/header.hpp"
#include "external/nipc/traits.hpp"

namespace ooe
{
//--- nipc ---------------------------------------------------------------------
	bool nipc::header_size( socket& socket, u32& size )
	{
		return socket.receive( &size, sizeof( u32 ) ) == sizeof( u32 );
	}

	nipc::array_type nipc::header_read( socket& socket, u32 size )
	{
		if ( !size )
			return array_type();

		array_type array( new u8[ size ] );

		if ( socket.receive( array, size ) != size )
			throw error::runtime( "nipc::header_read: " ) << "Unable to read data";

		return array;
	}

	void nipc::header_read( socket& socket, buffer_type& buffer, u32 size )
	{
		if ( !size )
			return;

		buffer.reserve( size );

		if ( socket.receive( &buffer[ 0 ], size ) != size )
			throw error::runtime( "nipc::header_read: " ) << "Unable to read data";
	}
}
