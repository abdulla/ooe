/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/nameservice.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe )( ipc ) )

up_t ipc_find( const any& any, io_buffer& buffer, pool& pool )
{
	std::string name;
	std::string type;
	stream_read< std::string, std::string >::call( buffer.get(), name, type );

	index_t index = static_cast< nameservice* >( any.pointer )->find( name, type );
	return return_write< index_t >( buffer, pool, index );
}

up_t ipc_list( const any& any, io_buffer& buffer, pool& pool )
{
	nameservice::list_type list = static_cast< nameservice* >( any.pointer )->list();
	return return_write< nameservice::list_type >( buffer, pool, list );
}

up_t ipc_find_all( const any& any, io_buffer& buffer, pool& pool )
{
	typedef std::vector< tuple< std::string, std::string > > in_type;
	in_type in;
	stream_read< in_type >::call( buffer.get(), in );

	typedef std::vector< index_t > out_type;
	out_type out;
	out.reserve( in.size() );

	ipc::nameservice& nameservice = *static_cast< ipc::nameservice* >( any.pointer );

	for ( in_type::const_iterator i = in.begin(), end = in.end(); i != end; ++i )
		out.push_back( nameservice.find( i->_0, i->_1 ) );

	return return_write< out_type >( buffer, pool, out );
}

up_t ipc_doc( const any& any, io_buffer& buffer, pool& pool )
{
	std::string name;
	std::string type;
	stream_read< std::string, std::string >::call( buffer.get(), name, type );

	std::string doc = static_cast< nameservice* >( any.pointer )->doc( name, type );
	return return_write< std::string >( buffer, pool, doc );
}

const up_t index_adjust = 5;

OOE_ANONYMOUS_NAMESPACE_END( ( ooe )( ipc ) )

OOE_NAMESPACE_BEGIN( ( ooe )( ipc ) )

//--- nameservice ----------------------------------------------------------------------------------
nameservice::nameservice( void )
	: switchboard(), vector(), map()
{
	if ( switchboard.insert_direct( ipc_find, this ) != 1 )
		throw error::runtime( "ipc::nameservice: " ) << "\"find\" not at index 1";
	else if ( switchboard.insert_direct( ipc_list, this ) != 2 )
		throw error::runtime( "ipc::nameservice: " ) << "\"list\" not at index 2";
	else if ( switchboard.insert_direct( ipc_find_all, this ) != 3 )
		throw error::runtime( "ipc::nameservice: " ) << "\"find_all\" not at index 3";
	else if ( switchboard.insert_direct( ipc_doc, this ) != 4 )
		throw error::runtime( "ipc::nameservice: " ) << "\"doc\" not at index 4";
}

nameservice::operator const ipc::switchboard&( void ) const
{
	return switchboard;
}

index_t nameservice::find( const std::string& name, const std::string& type ) const
{
	map_type::const_iterator i = map.find( map_tuple( name, type ) );
	return i == map.end() ? ~index_t( 0 ) : i->second;
}

nameservice::list_type nameservice::list( void ) const
{
	list_type value;
	value.reserve( map.size() );

	for ( map_type::const_iterator i = map.begin(), end = map.end(); i != end; ++i )
		value.push_back( i->first );

	return value;
}

std::string nameservice::doc( const std::string& name, const std::string& type ) const
{
	map_type::const_iterator i = map.find( map_tuple( name, type ) );

	if ( i == map.end() )
		throw error::runtime( "ipc::nameservice: " ) <<
			"Unable to find documentation for \"" << name << "\" of type \"" << type << '\"';

	up_t index = i->second - index_adjust;
	return vector[ index ] ? vector[ index ] : std::string();
}

void nameservice::insert_direct( const std::string& name, const std::string& type, const c8* doc_,
	index_t index )
{
	vector_type::iterator i = vector.begin();
	std::advance( i, index - index_adjust );
	vector.insert( i, doc_ );
	map.insert( map_type::value_type( map_tuple( name, type ), index ) );
}

void nameservice::insert_direct( const std::string& name, const std::string& type, const c8* doc_,
	switchboard::call_type call, any any )
{
	insert_direct( name, type, doc_, switchboard.insert_direct( call, any ) );
}

OOE_NAMESPACE_END( ( ooe )( ipc ) )
