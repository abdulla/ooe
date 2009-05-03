/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/memory/nameservice.hpp"

namespace
{
	using namespace ooe;

	void ipc_find( any any, const ipc::buffer_unpack& buffer_unpack, ipc::buffer_pack& buffer_pack,
		ipc::pool& )
	{
		const c8* name;
		const c8* type;
		ipc::layout_unpack< const c8*, const c8* >::call( buffer_unpack, name, type );

		u32 value = static_cast< ipc::nameservice* >( any.pointer )->find( name, type );
		ipc::layout_pack< u32 >::call( buffer_pack, value );
	}

	void ipc_list( any any, const ipc::buffer_unpack&, ipc::buffer_pack& buffer_pack, ipc::pool& )
	{
		ipc::nameservice::list_type value = static_cast< ipc::nameservice* >( any.pointer )->list();
		ipc::layout_pack< ipc::nameservice::list_type >::call( buffer_pack, value );
	}

	void ipc_find_all( any any, const ipc::buffer_unpack& buffer_unpack,
		ipc::buffer_pack& buffer_pack, ipc::pool& )
	{
		typedef std::vector< tuple< std::string, std::string > > in_type;
		in_type in;
		ipc::layout_unpack< in_type >::call( buffer_unpack, in );

		typedef std::vector< u32 > out_type;
		out_type out;
		out.reserve( in.size() );

		ipc::nameservice& nameservice = *static_cast< ipc::nameservice* >( any.pointer );

		for ( in_type::const_iterator i = in.begin(), end = in.end(); i != end; ++i )
			out.push_back( nameservice.find( i->_0, i->_1 ) );

		ipc::layout_pack< out_type >::call( buffer_pack, out );
	}
}

namespace ooe
{
//--- ipc::nameservice ---------------------------------------------------------
	ipc::nameservice::nameservice( void )
		: switchboard(), map()
	{
		if ( switchboard.insert_direct( ipc_find, this ) != 1 )
			throw error::runtime( "ipc::nameservice: " ) << "\"find\" not at index 1";

		if ( switchboard.insert_direct( ipc_list, this ) != 2 )
			throw error::runtime( "ipc::nameservice: " ) << "\"list\" not at index 2";

		if ( switchboard.insert_direct( ipc_find_all, this ) != 3 )
			throw error::runtime( "ipc::nameservice: " ) << "\"find_all\" not at index 3";
	}

	ipc::nameservice::operator const ipc::switchboard&( void ) const
	{
		return switchboard;
	}

	u32 ipc::nameservice::find( const std::string& name, const std::string& type ) const
	{
		map_type::const_iterator i = map.find( map_tuple( name, type ) );
		return i == map.end() ? -1 : i->second;
	}

	ipc::nameservice::list_type ipc::nameservice::list( void ) const
	{
		list_type value;
		value.reserve( map.size() );

		for ( map_type::const_iterator i = map.begin(), end = map.end(); i != end; ++i )
			value.push_back( i->first );

		return value;
	}

	void ipc::nameservice::
		insert_direct( const std::string& name, const std::string& type, u32 index )
	{
		map.insert( map_type::value_type( map_tuple( name, type ), index ) );
	}
}
