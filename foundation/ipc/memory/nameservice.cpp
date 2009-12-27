/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/memory/nameservice.hpp"

namespace
{
	using namespace ooe;

	void ipc_find( const any& any, u8* buffer_ptr, up_t buffer_size,
		ipc::memory::write_buffer& buffer, ipc::pool& )
	{
		const c8* name;
		const c8* type;
		ipc::stream_read< const c8*, const c8* >::call( header_adjust( buffer ), name, type );

		u32 value = static_cast< ipc::memory::nameservice* >( any.pointer )->find( name, type );
		ipc::memory::return_write< u32 >( buffer_ptr, buffer_size, buffer, value );
	}

	void ipc_list( const any& any, u8* buffer_ptr, up_t buffer_size,
		ipc::memory::write_buffer& buffer, ipc::pool& )
	{
		typedef ipc::memory::nameservice::list_type list_type;
		list_type value = static_cast< ipc::memory::nameservice* >( any.pointer )->list();
		ipc::memory::return_write< list_type >( buffer_ptr, buffer_size, buffer, value );
	}

	void ipc_find_all( const any& any, u8* buffer_ptr, up_t buffer_size,
		ipc::memory::write_buffer& buffer, ipc::pool& )
	{
		typedef std::vector< ooe::tuple< std::string, std::string > > in_type;
		in_type in;
		ipc::stream_read< in_type >::call( header_adjust( buffer ), in );

		typedef std::vector< u32 > out_type;
		out_type out;
		out.reserve( in.size() );

		ipc::memory::nameservice& nameservice =
			*static_cast< ipc::memory::nameservice* >( any.pointer );

		for ( in_type::const_iterator i = in.begin(), end = in.end(); i != end; ++i )
			out.push_back( nameservice.find( i->_0, i->_1 ) );

		ipc::memory::return_write< out_type >( buffer_ptr, buffer_size, buffer, out );
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
		else if ( switchboard.insert_direct( ipc_list, this ) != 2 )
			throw error::runtime( "ipc::nameservice: " ) << "\"list\" not at index 2";
		else if ( switchboard.insert_direct( ipc_find_all, this ) != 3 )
			throw error::runtime( "ipc::nameservice: " ) << "\"find_all\" not at index 3";
	}

	ipc::memory::nameservice::operator const memory::switchboard&( void ) const
	{
		return switchboard;
	}

	u32 ipc::memory::nameservice::find( const std::string& name, const std::string& type ) const
	{
		map_type::const_iterator i = map.find( map_tuple( name, type ) );
		return i == map.end() ? ~u32( 0 ) : i->second;
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

	void ipc::memory::nameservice::insert_direct
		( const std::string& name, const std::string& type, switchboard::call_type call, any any )
	{
		insert_direct( name, type, switchboard.insert_direct( call, any ) );
	}
}
