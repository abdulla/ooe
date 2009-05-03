/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/utility/error.hpp"

#include "external/nipc/nameservice.hpp"

namespace
{
	using namespace ooe;

	void nipc_find( any any, socket& socket, u8* buffer, nipc::pool& )
	{
		const c8* name;
		const c8* type;
		nipc::layout_read< const c8*, const c8* >::call( buffer, name, type );

		u32 value = static_cast< nipc::nameservice* >( any.pointer )->find( name, type );
		nipc::layout_write< u32 >::call( socket, value );
	}
}

namespace ooe
{
//--- nipc::nameservice --------------------------------------------------------
	nipc::nameservice::nameservice( void )
		: switchboard(), map()
	{
		if ( switchboard.insert_direct( nipc_find, this ) != 1 )
			throw error::runtime( "nipc::nameservice: " ) << "\"find\" not at index 0";
	}

	nipc::nameservice::operator const nipc::switchboard&( void ) const
	{
		return switchboard;
	}

	u32 nipc::nameservice::find( const std::string& name, const std::string& type ) const
	{
		map_type::const_iterator i = map.find( map_tuple( name, type ) );
		return i == map.end() ? -1 : i->second;
	}

	void nipc::nameservice::
		insert_direct( const std::string& name, const std::string& type, u32 index )
	{
		map.insert( map_type::value_type( map_tuple( name, type ), index ) );
	}
}
