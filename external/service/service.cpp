/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "external/service/service.hpp"

namespace ooe
{
//--- service ------------------------------------------------------------------
	void service::insert( function_type function, void* pointer )
	{
		vector.push_back( vector_type::value_type( function, pointer ) );
	}

	void service::insert( const std::string& name, signal_base& signal )
	{
		map.insert( map_type::value_type( name, &signal ) );
	}

	signal_base* service::operator []( const std::string& name ) const
	{
		map_type::const_iterator i = map.find( name );
		return i == map.end() ? 0 : i->second;
	}

	void service::operator ()( void ) const
	{
		for ( vector_type::const_iterator i = vector.begin(), end = vector.end(); i != end; ++i )
			i->_0( i->_1 );
	}
}
