/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/pool.hpp"
#include "foundation/utility/convert.hpp"

namespace ooe
{
//--- ipc::pool ----------------------------------------------------------------
	ipc::pool::~pool( void )
	{
		typedef map_type::const_iterator iterator;

		for ( iterator i = map.begin(), end = map.end(); i != end; ++i )
			i->second( i->first );
	}

	void ipc::pool::insert( const void* p, function_type f )
	{
		map.insert( map_type::value_type( p, f ) );
	}

	void ipc::pool::erase( const void* p )
	{
		map_type::iterator i = map.find( p );

		if ( i == map.end() )
			throw error::runtime( "pool: " ) << "Unknown pointer: " << ptr( p );

		map.erase( i );
	}

	bool ipc::pool::find( const void* p ) const
	{
		return map.find( p ) != map.end();
	}
}
