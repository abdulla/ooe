/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <algorithm>

#include "component/registry/remote.hpp"

namespace ooe
{
//--- facade::remote -----------------------------------------------------------
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

//--- remote -------------------------------------------------------------------
	remote::remote( const std::string& path )
		: client( path )
	{
	}

	bool remote::supports( const interface& interface ) const
	{
		typedef ipc::memory::rpc< std::vector< u32 > ( const interface::vector_type& ) > rpc_type;
		typedef rpc_type::result_type result_type;
		result_type result = rpc_type( client, 3 )( interface.get() );

		for ( result_type::const_iterator i = result.begin(), end = result.end(); i != end; ++i )
		{
			if ( *i == ~u32( 0 ) )
				return false;
		}

		return true;
	}
}
