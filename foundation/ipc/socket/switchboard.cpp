/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/utility/convert.hpp"
#include "foundation/utility/error.hpp"

#include "foundation/ipc/socket/switchboard.hpp"

namespace
{
	using namespace ooe;

	void ipc_null( const any&, const u8*, const buffer_tuple&, socket& socket, ipc::pool& )
	{
		u32 size = 0;
		
		if ( socket.send( &size, sizeof( u32 ) ) != 0 )
			throw error::runtime( "ipc::socket::switchboard: " ) << "Unable to write size";
	}
}

namespace ooe
{
//--- ipc::socket::switchboard -------------------------------------------------
	ipc::socket::switchboard::switchboard( void )
		: vector(), buffer()
	{
		insert_direct( ipc_null, 0 );
	}

	void ipc::socket::switchboard::execute( const u8* data, ooe::socket& socket, pool& pool ) const
	{
		try
		{
			u32 index;
			data += read< u32 >::call( data, index );

			if ( index >= vector.size() )
				throw error::runtime( "ipc::socket::switchboard: " ) <<
					"Unable to execute function, index " << index << " out of range";

			const vector_tuple& tuple = vector[ index ];
			tuple._0( tuple._1, data, buffer_tuple( buffer, sizeof( buffer ) ), socket, pool );
		}
		catch ( error::runtime& error )
		{
			write< u32 >::call( socket, -1, sizeof( u32 ) );
			layout_write< const c8*, const c8* >::call( socket, error.what(), error.where() );
		}
		catch ( std::exception& error )
		{
			write< u32 >::call( socket, -1, sizeof( u32 ) );
			layout_write< const c8*, const c8* >::call( socket, error.what(),
				"\nNo stack trace available" );
		}
		catch ( ... )
		{
			write< u32 >::call( socket, -1, sizeof( u32 ) );
			layout_write< const c8*, const c8* >::call( socket,
				"An unknown exception was thrown", "\nNo stack trace available" );
		}
	}

	u32 ipc::socket::switchboard::insert_direct( call_type call, any any )
	{
		vector.push_back( vector_tuple( call, any ) );
		return vector.size() - 1;
	}
}
