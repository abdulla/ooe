/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/socket/nameservice.hpp"

namespace
{
	using namespace ooe;

	void ipc_find( const any& any, const u8* data, u8* buffer_ptr, up_t buffer_size, socket& socket,
		ipc::pool& )
	{
		const c8* name;
		const c8* type;
		ipc::stream_read< const c8*, const c8* >::call( data, name, type );

		u32 value = static_cast< ipc::socket::nameservice* >( any.pointer )->find( name, type );
		ipc::socket::return_write< u32 >( buffer_ptr, buffer_size, socket, value );
	}
}

namespace ooe
{
//--- ipc::socket::nameservice --------------------------------------------------------
	ipc::socket::nameservice::nameservice( void )
		: switchboard(), map()
	{
		if ( switchboard.insert_direct( ipc_find, this ) != 1 )
			throw error::runtime( "ipc::socket::nameservice: " ) << "\"find\" not at index 0";
	}

	ipc::socket::nameservice::operator const ipc::socket::switchboard&( void ) const
	{
		return switchboard;
	}

	u32 ipc::socket::nameservice::find( const std::string& name, const std::string& type ) const
	{
		map_type::const_iterator i = map.find( map_tuple( name, type ) );
		return i == map.end() ? ~u32( 0 ) : i->second;
	}

	void ipc::socket::nameservice::
		insert_direct( const std::string& name, const std::string& type, u32 index )
	{
		map.insert( map_type::value_type( map_tuple( name, type ), index ) );
	}
}
