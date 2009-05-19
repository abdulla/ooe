/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/environment.hpp"
#include "foundation/ipc/socket/switchboard.hpp"
#include "foundation/utility/convert.hpp"
#include "foundation/utility/error.hpp"

namespace
{
	using namespace ooe;

	void ipc_null( const any&, const u8*, const ipc::socket::buffer_tuple& tuple, socket& socket,
		ipc::pool& )
	{
		ipc::socket::return_write( tuple, socket );
	}

	void error_write( const ipc::socket::buffer_tuple& tuple, socket& socket, const c8* what,
		const c8* where )
	{
		up_t size = ipc::stream_size< const c8*, const c8* >::call( what, where );
		up_t total = size + sizeof( u32 ) * 2;
		ipc::socket::write_buffer buffer( tuple, total );
		u8* data = buffer.get();
		ipc::stream_write< u32, u32, const c8*, const c8* >::call( data, -1, size, what, where );

		if ( socket.send( data, total ) != total )
			throw error::runtime( "ipc::socket::switchboard: " ) << "Unable to write data";
	}
}

namespace ooe
{
//--- ipc::socket::switchboard -------------------------------------------------
	ipc::socket::switchboard::switchboard( void )
		: vector()
	{
		insert_direct( ipc_null, 0 );
	}

	void ipc::socket::switchboard::execute( const u8* data, ooe::socket& socket, pool& pool ) const
	{
		u8 buffer[ executable::static_page_size ];
		buffer_tuple tuple( buffer, sizeof( buffer ) );

		try
		{
			u32 index;
			data += read< u32 >::call( data, index );

			if ( index >= vector.size() )
				throw error::runtime( "ipc::socket::switchboard: " ) <<
					"Unable to execute function, index " << index << " out of range";

			const vector_tuple& args = vector[ index ];
			args._0( args._1, data, tuple, socket, pool );
		}
		catch ( error::runtime& error )
		{
			error_write( tuple, socket, error.what(), error.where() );
		}
		catch ( std::exception& error )
		{
			error_write( tuple, socket, error.what(), "\nNo stack trace available" );
		}
		catch ( ... )
		{
			error_write( tuple, socket,
				"An unknown exception was thrown", "\nNo stack trace available" );
		}
	}

	u32 ipc::socket::switchboard::insert_direct( call_type call, any any )
	{
		vector.push_back( vector_tuple( call, any ) );
		return vector.size() - 1;
	}
}
