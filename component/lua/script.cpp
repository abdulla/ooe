/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "component/lua/script.hpp"

namespace ooe
{
//--- facade::lua --------------------------------------------------------------
	const facade::lua::vector_type& facade::lua::get( void ) const
	{
		return vector;
	}

	void facade::lua::insert( up_t index, ooe::lua::cfunction call )
	{
		vector_type::iterator i = vector.begin();
		std::advance( i, index );
		vector.insert( i, call );
	}
}
