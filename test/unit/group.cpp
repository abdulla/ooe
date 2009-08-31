/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "test/unit/group.hpp"

namespace ooe
{
//--- unit::group_base ---------------------------------------------------------
	unit::group_base::iterator_type unit::group_base::begin( void ) const
	{
		return vector.begin();
	}

	unit::group_base::iterator_type unit::group_base::end( void ) const
	{
		return vector.end();
	}

	void unit::group_base::push_back( function_type function )
	{
		vector.push_back( function );
	}
}