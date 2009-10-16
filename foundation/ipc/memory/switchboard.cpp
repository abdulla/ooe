/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/memory/header.hpp"
#include "foundation/ipc/memory/switchboard.hpp"

namespace
{
	using namespace ooe;

	void ipc_null( const any&, u8*, up_t, ipc::memory::write_buffer&, ipc::pool& )
	{
	}

	void return_error( u8* buffer_ptr, up_t buffer_size, ipc::memory::write_buffer& buffer,
		bool executed, const c8* what, const c8* where )
	{
		up_t size = ipc::stream_size< bool, const c8*, const c8* >::call( executed, what, where );
		u8* data = return_write( buffer_ptr, buffer_size, buffer, size, error::exception );
		ipc::stream_write< bool, const c8*, const c8* >::call( data, executed, what, where );
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
		execute( u8* buffer_ptr, up_t buffer_size, write_buffer& buffer, pool& pool ) const
	{
		buffer_ptr = header_adjust( buffer_ptr );
		buffer_size = header_adjust( buffer_size );
		bool executed = false;

		try
		{
			write_buffer( header_read( buffer_ptr ), buffer_ptr ).swap( buffer );

			u32 index;
			stream_read< u32 >::call( buffer.get(), index );

			if ( index >= vector.size() )
				throw error::runtime( "ipc::switchboard: " ) <<
					"Unable to execute function, index " << index << " out of range";

			const vector_tuple& args = vector[ index ];
			executed = true;
			args._0( args._1, buffer_ptr, buffer_size, buffer, pool );
		}
		catch ( ipc::migration& )
		{
			throw;
		}
		catch ( error::verification& error )
		{
			return_error( buffer_ptr, buffer_size, buffer, false, error.what(), error.where() );
		}
		catch ( error::runtime& error )
		{
			return_error( buffer_ptr, buffer_size, buffer, executed, error.what(), error.where() );
		}
		catch ( std::exception& error )
		{
			return_error( buffer_ptr, buffer_size, buffer, executed, error.what(),
				"\nNo stack trace available" );
		}
		catch ( ... )
		{
			return_error( buffer_ptr, buffer_size, buffer, executed,
				"An unknown exception was thrown", "\nNo stack trace available" );
		}

		header_write( buffer_ptr, buffer );
	}

	u32 ipc::memory::switchboard::insert_direct( call_type call, any any )
	{
		vector.push_back( vector_tuple( call, any ) );
		return vector.size() - 1;
	}
}
