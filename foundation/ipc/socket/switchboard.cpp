/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/utility/convert.hpp"
#include "foundation/utility/error.hpp"

#include "external/nipc/switchboard.hpp"

namespace
{
	using namespace ooe;

	void nipc_null( any, socket& socket, u8*, nipc::pool& )
	{
		nipc::layout_write<>::call( socket );
	}
}

namespace ooe
{
//--- nipc::switchboard -----------------------------------------------------------
	nipc::switchboard::switchboard( void )
		: vector()
	{
		insert_direct( nipc_null, 0 );
	}

	void nipc::switchboard::execute( socket& socket, u8* buffer, pool& pool ) const
	{
		u32 index;
		layout_read< u32 >::call( &buffer[ 0 ], index );

		try
		{
			if ( index >= vector.size() )
				throw error::runtime( "nipc::switchboard: " ) <<
					"Unable to execute function, index " << index << " out of range";

			const vector_tuple& tuple = vector[ index ];
			tuple._0( tuple._1, socket, buffer + sizeof( u32 ), pool );
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

	u32 nipc::switchboard::insert_direct( call_type call, any any )
	{
		vector.push_back( vector_tuple( call, any ) );
		return vector.size() - 1;
	}
}
