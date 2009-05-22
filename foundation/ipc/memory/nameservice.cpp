/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/memory/nameservice.hpp"

namespace
{
	using namespace ooe;

	void ipc_find( const any& any, const u8* data, const ipc::memory::buffer_tuple& tuple,
		ipc::memory::write_buffer& buffer, ipc::pool& )
	{
		const c8* name;
		const c8* type;
		ipc::stream_read< const c8*, const c8* >::call( data, name, type );

		u32 value = static_cast< ipc::memory::nameservice* >( any.pointer )->find( name, type );
		up_t size = ipc::stream_size< u32 >::call( value );
		ipc::stream_write< u32 >::call( return_write( tuple, buffer, size ), value );
	}

	void ipc_list( const any& any, const u8*, const ipc::memory::buffer_tuple& tuple,
		ipc::memory::write_buffer& buffer, ipc::pool& )
	{
		typedef ipc::memory::nameservice::list_type list_type;
		list_type value = static_cast< ipc::memory::nameservice* >( any.pointer )->list();
		up_t size = ipc::stream_size< list_type >::call( value );
		ipc::stream_write< list_type >::call( return_write( tuple, buffer, size ), value );
	}

	void ipc_find_all( const any& any, const u8* data, const ipc::memory::buffer_tuple& tuple,
		ipc::memory::write_buffer& buffer, ipc::pool& )
	{
		typedef std::vector< ooe::tuple< std::string, std::string > > in_type;
		in_type in;
		ipc::stream_read< in_type >::call( data, in );

		typedef std::vector< u32 > out_type;
		out_type out;
		out.reserve( in.size() );

		ipc::memory::nameservice& nameservice =
			*static_cast< ipc::memory::nameservice* >( any.pointer );

		for ( in_type::const_iterator i = in.begin(), end = in.end(); i != end; ++i )
			out.push_back( nameservice.find( i->_0, i->_1 ) );

		up_t size = ipc::stream_size< out_type >::call( out );
		ipc::stream_write< out_type >::call( return_write( tuple, buffer, size ), out );
	}
}

namespace ooe
{
//--- ipc::memory::nameservice ---------------------------------------------------------
	ipc::memory::nameservice::nameservice( void )
		: switchboard(), map()
	{
		if ( switchboard.insert_direct( ipc_find, this ) != 1 )
			throw error::runtime( "ipc::nameservice: " ) << "\"find\" not at index 1";

		if ( switchboard.insert_direct( ipc_list, this ) != 2 )
			throw error::runtime( "ipc::nameservice: " ) << "\"list\" not at index 2";

		if ( switchboard.insert_direct( ipc_find_all, this ) != 3 )
			throw error::runtime( "ipc::nameservice: " ) << "\"find_all\" not at index 3";
	}

	ipc::memory::nameservice::operator const memory::switchboard&( void ) const
	{
		return switchboard;
	}

	u32 ipc::memory::nameservice::find( const std::string& name, const std::string& type ) const
	{
		map_type::const_iterator i = map.find( map_tuple( name, type ) );
		return i == map.end() ? -1 : i->second;
	}

	ipc::memory::nameservice::list_type ipc::memory::nameservice::list( void ) const
	{
		list_type value;
		value.reserve( map.size() );

		for ( map_type::const_iterator i = map.begin(), end = map.end(); i != end; ++i )
			value.push_back( i->first );

		return value;
	}

	void ipc::memory::nameservice::
		insert_direct( const std::string& name, const std::string& type, u32 index )
	{
		map.insert( map_type::value_type( map_tuple( name, type ), index ) );
	}
}
