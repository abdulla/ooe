/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/memory/header.hpp"
#include "foundation/ipc/memory/switchboard.hpp"

namespace
{
	using namespace ooe;

	void ipc_null( const any&, const u8*, const ipc::buffer_tuple&, ipc::buffer_type&, ipc::pool& )
	{
	}

	void return_error
		( const ipc::buffer_tuple& tuple, ipc::buffer_type& buffer, const std::string& string )
	{
		up_t size = ipc::stream_size< std::string >::call( string );
		u8* data = return_write( tuple, buffer, size, error::exception );
		ipc::stream_write< std::string >::call( data, string );
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

	void ipc::switchboard::
		execute( const buffer_tuple& tuple, buffer_type& buffer, pool& pool ) const
	{
		buffer_tuple adjust = header_adjust( tuple );

		try
		{
			buffer_type input( header_read( adjust._0 ), adjust._0 );

			u32 index;
			u8* data = input.get();
			data += read< u32 >::call( data, index );

			if ( index >= vector.size() )
				throw error::runtime( "ipc::switchboard: " ) <<
					"Unable to execute function, index " << index << " out of range";

			const vector_tuple& args = vector[ index ];
			args._0( args._1, data, adjust, buffer, pool );
		}
		catch ( error::runtime& error )
		{
			std::string string;
			string << error.what() << "\n\nServer stack trace:" << error.where();
			return_error( tuple, buffer, string );
		}
		catch ( std::exception& error )
		{
			std::string string;
			string << error.what() << "\n\nServer stack trace:" << "\nNo stack trace available";
			return_error( tuple, buffer, string );
		}
		catch ( ... )
		{
			std::string string;
			string << "An unknown exception was thrown" <<
				"\n\nServer stack trace:" << "\nNo stack trace available";
			return_error( tuple, buffer, string );;
		}

		header_write( adjust._0, buffer );
	}

	u32 ipc::switchboard::insert_direct( call_type call, any any )
	{
		vector.push_back( vector_tuple( call, any ) );
		return vector.size() - 1;
	}
}
