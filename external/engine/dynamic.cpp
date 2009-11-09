/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "external/engine/dynamic.hpp"

namespace ooe
{
//--- dynamic ------------------------------------------------------------------
	tuple< up_t, up_t > dynamic::size( void ) const
	{
		return make_tuple( map.size(), vector.size() );
	}

	library& dynamic::load( const std::string name, library::type flag )
	{
		map_type::const_iterator i = map.find( name );

		if ( i != map.end() )
			return *i->second;

		library_ptr pointer = new library( name + OOE_EXTENSION, flag );
		map.insert( map_type::value_type( name, pointer ) );
		return *pointer;
	}

	void dynamic::clear( void )
	{
		typedef vector_type::const_reverse_iterator iterator;

		for ( iterator i = vector.rbegin(), end = vector.rend(); i != end; ++i )
			i->_1( i->_0 );

		vector.clear();
		map.clear();
	}
}
