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
}
