/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <algorithm>

#include "component/registry/interface.hpp"
#include "foundation/utility/error.hpp"

namespace ooe
{
//--- interface ----------------------------------------------------------------
	const interface::vector_type& interface::get( void ) const
	{
		return vector;
	}

	void interface::insert( const std::string& name, const std::string& type )
	{
		vector_tuple tuple( name, type );
		vector_type::iterator i = std::lower_bound( vector.begin(), vector.end(), tuple );

		if ( i != vector.end() && *i == tuple )
			throw error::runtime( "interface: " ) <<
				"Unable to insert \"" << name << "\" of type \"" << type << "\", function exists";

		vector.insert( i, tuple );
	}
}
