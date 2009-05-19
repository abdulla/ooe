/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/socket/nameservice.hpp"

namespace
{
	using namespace ooe;

	void ipc_find( any any, socket& socket, u8* buffer, ipc::pool& )
	{
		const c8* name;
		const c8* type;
		ipc::stream_read< const c8*, const c8* >::call( buffer, name, type );

		u32 value = static_cast< nipc::nameservice* >( any.pointer )->find( name, type );
		ipc::streamt_write< u32 >::call( socket, value );
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
		return i == map.end() ? -1 : i->second;
	}

	void ipc::socket::nameservice::
		insert_direct( const std::string& name, const std::string& type, u32 index )
	{
		map.insert( map_type::value_type( map_tuple( name, type ), index ) );
	}
}
