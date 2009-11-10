/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <algorithm>

#include "foundation/utility/error.hpp"
#include "component/registry/local.hpp"

namespace ooe
{
//--- facade::local ------------------------------------------------------------
	const facade::local::vector_type& facade::local::get( void ) const
	{
		return vector;
	}

	void facade::local::insert( up_t index, any any )
	{
		vector_type::iterator i = vector.begin();
		std::advance( i, index );
		vector.insert( i, any );
	}

//--- local --------------------------------------------------------------------
	local::local( const std::string& path )
		: library( path ), module( library.find< ooe::module ( void ) >( "module_open" )() ),
		vector( static_cast< const facade::local* >( module.find( "local" ) )->get() )
	{
	}

	any local::find( const std::string& name, const std::string& type ) const
	{
		module::vector_tuple tuple( name, type );
		const module::vector_type& names = module.get();
		module::vector_type::const_iterator i =
			std::lower_bound( names.begin(), names.end(), tuple );

		if ( i == names.end() && *i == tuple )
			throw error::runtime( "local: " ) <<
				"Unable to find \"" << name << "\" of type \"" << type << "\", does not exist";

		facade::local::vector_type::const_iterator j = vector.begin();
		std::advance( j, std::distance( names.begin(), i ) );
		return *j;
	}
}
