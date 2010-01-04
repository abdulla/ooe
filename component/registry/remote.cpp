/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <algorithm>

#include "component/registry/remote.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( facade ) )

//--- remote ---------------------------------------------------------------------------------------
const facade::remote::vector_type& facade::remote::get( void ) const
{
	return vector;
}

void facade::remote::insert( up_t index, call_type call )
{
	vector_type::iterator i = vector.begin();
	std::advance( i, index );
	vector.insert( i, call );
}

OOE_NAMESPACE_END( ( ooe )( facade ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- remote ---------------------------------------------------------------------------------------
remote::remote( const std::string& path )
	: client( path )
{
}

bool remote::supports( const interface& interface ) const
{
	typedef ipc::memory::rpc< std::vector< ipc::index_t > ( const interface::vector_type& ) >
		rpc_type;
	typedef rpc_type::result_type result_type;
	result_type result = rpc_type( client, 3 )( interface.get() );

	for ( result_type::const_iterator i = result.begin(), end = result.end(); i != end; ++i )
	{
		if ( *i == ~ipc::index_t( 0 ) )
			return false;
	}

	return true;
}

OOE_NAMESPACE_END( ( ooe ) )
