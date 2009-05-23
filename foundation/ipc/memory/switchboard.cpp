/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/memory/header.hpp"
#include "foundation/ipc/memory/switchboard.hpp"

namespace
{
	using namespace ooe;

	void ipc_null( const any&, const ipc::memory::buffer_tuple&, ipc::memory::write_buffer&,
		ipc::pool& )
	{
	}

	void return_error( const ipc::memory::buffer_tuple& tuple, ipc::memory::write_buffer& buffer,
		const c8* what, const c8* where )
	{
		up_t size = ipc::stream_size< const c8*, const c8* >::call( what, where );
		u8* data = return_write( tuple, buffer, size, error::exception );
		ipc::stream_write< const c8*, const c8* >::call( data, what, where );
	}
}

namespace ooe
{
//--- ipc::memory::switchboard -------------------------------------------------
	ipc::memory::switchboard::switchboard( void )
		: vector()
	{
		insert_direct( ipc_null, 0 );
	}

	void ipc::memory::switchboard::
		execute( const buffer_tuple& adjust, write_buffer& buffer, pool& pool ) const
	{
		buffer_tuple tuple = header_adjust( adjust );

		try
		{
			write_buffer( header_read( tuple._0 ), tuple._0 ).swap( buffer );

			u32 index;
			stream_read< u32 >::call( buffer.get(), index );

			if ( index >= vector.size() )
				throw error::runtime( "ipc::switchboard: " ) <<
					"Unable to execute function, index " << index << " out of range";

			const vector_tuple& args = vector[ index ];
			args._0( args._1, tuple, buffer, pool );
		}
		catch ( error::runtime& error )
		{
			return_error( tuple, buffer, error.what(), error.where() );
		}
		catch ( std::exception& error )
		{
			return_error( tuple, buffer, error.what(), "\nNo stack trace available" );
		}
		catch ( ... )
		{
			return_error( tuple, buffer, "An unknown exception was thrown",
				"\nNo stack trace available" );
		}

		header_write( tuple._0, buffer );
	}

	u32 ipc::memory::switchboard::insert_direct( call_type call, any any )
	{
		vector.push_back( vector_tuple( call, any ) );
		return vector.size() - 1;
	}
}
