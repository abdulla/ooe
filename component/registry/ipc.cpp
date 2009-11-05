/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <algorithm>

#include "component/registry/ipc.hpp"

namespace
{
	using namespace ooe;

	const std::string& validate( const module::info_tuple& info )
	{
		if ( info._0 != module::server )
			throw error::runtime( "internal: " ) << "Info " << info << " does not name a server";

		return info._1;
	}
}

namespace ooe
{
//--- facade::ipc --------------------------------------------------------------
	void facade::ipc::insert( up_t index, call_type call )
	{
		vector_type::iterator i = vector.begin();
		std::advance( i, index );
		vector.insert( i, call );
	}

//--- external -----------------------------------------------------------------
	external::external( const module::info_tuple& info )
		: client( validate( info ) )
	{
	}
}
