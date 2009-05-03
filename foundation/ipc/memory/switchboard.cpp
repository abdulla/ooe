/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/error.hpp"
#include "foundation/ipc/switchboard.hpp"
#include "foundation/ipc/traits.hpp"

namespace
{
	using namespace ooe;

	void ipc_null( any, const ipc::buffer_unpack&, ipc::buffer_pack&, ipc::pool& )
	{
	}
}

namespace ooe
{
//--- ipc::switchboard ---------------------------------------------------------
	ipc::switchboard::switchboard( void )
		: vector()
	{
		insert_direct( ipc_null, 0 );
	}

	void ipc::switchboard::execute( u8* buffer, up_t size, memory_type& memory, pool& pool ) const
	{
		header_type& header = *reinterpret_cast< header_type* >( buffer );
		buffer_pack buffer_pack( buffer, size, sizeof( header_type ) );

		try
		{
			if ( header._0 >= vector.size() )
				throw error::runtime( "ipc::switchboard: " ) <<
					"Unable to execute function, index " << header._0 << " out of range";

			buffer_unpack buffer_unpack( buffer, sizeof( header_type ), header, memory );
			const vector_tuple& tuple = vector[ header._0 ];
			tuple._0( tuple._1, buffer_unpack, buffer_pack, pool );
			header._0 = error::none;
		}
		catch ( error::runtime& error )
		{
			header._0 = error::exception;
			layout_pack< const c8*, const c8* >::call( buffer_pack, error.what(), error.where() );
		}
		catch ( std::exception& error )
		{
			header._0 = error::exception;
			layout_pack< const c8*, const c8* >::call( buffer_pack, error.what(),
				"\nNo stack trace available" );
		}
		catch ( ... )
		{
			header._0 = error::exception;
			layout_pack< const c8*, const c8* >::call( buffer_pack,
				"An unknown exception was thrown", "\nNo stack trace available" );
		}

		delete memory.release();
		buffer_pack.store( header, memory );
	}

	u32 ipc::switchboard::insert_direct( call_type call, any any )
	{
		vector.push_back( vector_tuple( call, any ) );
		return vector.size() - 1;
	}
}
