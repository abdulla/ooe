/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "component/javascript/facade.hpp"

namespace ooe
{
//--- facade::javascript -------------------------------------------------------
	const facade::javascript::vector_type& facade::javascript::get( void ) const
	{
		return vector;
	}

	void facade::javascript::insert( up_t index, v8::InvocationCallback call )
	{
		vector_type::iterator i = vector.begin();
		std::advance( i, index );
		vector.insert( i, call );
	}
}
