/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_EXTERNAL_SHOW_EXPOSE_HPP
#define OOE_EXTERNAL_SHOW_EXPOSE_HPP

#include "external/graph/mesh.hpp"
#include "external/graph/resource.hpp"

namespace ooe
{
	void expose( lua::stack&, cache_DEPRECATED< texture >&, cache_DEPRECATED< mesh >& );
}

#endif	// OOE_EXTERNAL_SHOW_EXPOSE_HPP
