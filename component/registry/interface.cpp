/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "component/registry/interface.hpp"

namespace ooe
{
//--- interface ----------------------------------------------------------------
	const interface::name_vector& interface::names( void ) const
	{
		return names_;
	}
}
