/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <algorithm>

#include "component/registry/module.hpp"
#include "foundation/utility/error.hpp"

namespace ooe
{
//--- facade_id ----------------------------------------------------------------
	facade_id::facade_id( const void* pointer_, function_type function_ )
		: pointer( pointer_ ), function( function_ )
	{
	}

	facade_id::~facade_id( void )
	{
		function( pointer );
	}

//--- module -------------------------------------------------------------------
	const module::vector_type& module::get( void ) const
	{
		return vector;
	}

	up_t module::insert( const std::string& name, const std::string& type )
	{
		vector_tuple tuple( name, type );
		vector_type::iterator i = std::lower_bound( vector.begin(), vector.end(), tuple );

		if ( i != vector.end() && *i == tuple )
			throw error::runtime( "module: " ) <<
				"Unable to insert \"" << name << "\" of type \"" << type << "\", function exists";

		up_t distance = std::distance( vector.begin(), i );
		vector.insert( i, tuple );
		return distance;
	}

	const void* module::find( const std::string& name ) const
	{
		map_type::const_iterator i = map.find( name );

		if ( i == map.end() || i->first != name )
			throw error::runtime( "module: " ) <<
				"Unable to find \"" << name << "\", does not exist";

		return i->second->pointer;
	}

	void module::insert( const std::string& name, const facade_ptr& pointer )
	{
		map_type::const_iterator i = map.find( name );

		if ( i != map.end() )
			throw error::runtime( "module: " ) <<
				"Unable to insert \"" << name << "\", facade exists";

		map.insert( map_type::value_type( name, pointer ) );
	}
}
