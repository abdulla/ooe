/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/pool.hpp"

namespace ooe
{
//--- ipc::pool ----------------------------------------------------------------
	ipc::pool::~pool( void )
	{
		typedef map_type::const_iterator iterator;

		for ( iterator i = map.begin(), end = map.end(); i != end; ++i )
			i->second( i->first );
	}

	void ipc::pool::insert( const void* pointer, function_type function )
	{
		map.insert( map_type::value_type( pointer, function ) );
	}

	void ipc::pool::erase( const void* pointer )
	{
		map.erase( pointer );
	}

	bool ipc::pool::find( const void* pointer ) const
	{
		return map.find( pointer ) != map.end();
	}
}
