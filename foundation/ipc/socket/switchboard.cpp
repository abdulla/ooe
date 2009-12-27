/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/environment.hpp"
#include "foundation/ipc/socket/switchboard.hpp"
#include "foundation/utility/convert.hpp"
#include "foundation/utility/error.hpp"

namespace
{
	using namespace ooe;

	void ipc_null( const any&, const u8*, u8* buffer_ptr, up_t, socket& socket, ipc::pool& )
	{
		ipc::socket::return_write( buffer_ptr, socket );
	}

	void return_error( u8* buffer_ptr, up_t buffer_size, socket& socket, bool executed,
		const c8* what, const c8* where )
	{
		up_t size = ipc::stream_size< bool, const c8*, const c8* >::call( executed, what, where );
		up_t total = size + sizeof( u32 ) * 2;
		ipc::socket::write_buffer buffer( buffer_ptr, buffer_size, total );
		u8* data = buffer.get();
		ipc::stream_write< u32, u32, bool, const c8*, const c8* >::
			call( data, ~u32( 0 ), size, executed, what, where );

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
		bool executed = false;

		try
		{
			u32 index;
			data += read< u32 >::call( data, index );

			if ( index >= vector.size() )
				throw error::runtime( "ipc::socket::switchboard: " ) <<
					"Unable to execute function, index " << index << " out of range";

			const vector_tuple& args = vector[ index ];
			executed = true;
			args._0( args._1, data, buffer, sizeof( buffer ), socket, pool );
		}
		catch ( error::verification& error )
		{
			return_error( buffer, sizeof( buffer ), socket, false, error.what(), error.where() );
		}
		catch ( error::runtime& error )
		{
			return_error( buffer, sizeof( buffer ), socket, executed, error.what(), error.where() );
		}
		catch ( std::exception& error )
		{
			return_error( buffer, sizeof( buffer ), socket, executed, error.what(),
				"\nNo stack trace available" );
		}
		catch ( ... )
		{
			return_error( buffer, sizeof( buffer ), socket, executed,
				"An unknown exception was thrown", "\nNo stack trace available" );
		}
	}

	u32 ipc::socket::switchboard::insert_direct( call_type call, any any )
	{
		vector.push_back( vector_tuple( call, any ) );
		return vector.size() - 1;
	}
}
