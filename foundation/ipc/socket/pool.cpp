/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/socket/pool.hpp"

namespace ooe
{
//--- nipc::pool ---------------------------------------------------------------
	nipc::pool::~pool( void )
	{
		typedef map_type::const_iterator iterator;

		for ( iterator i = map.begin(), end = map.end(); i != end; ++i )
			i->second( i->first );
	}

	void nipc::pool::insert( void* pointer, function_type function )
	{
		map.insert( map_type::value_type( pointer, function ) );
	}

	void nipc::pool::erase( void* pointer )
	{
		map.erase( pointer );
	}

	bool nipc::pool::find( void* pointer ) const
	{
		return map.find( pointer ) != map.end();
	}
}
