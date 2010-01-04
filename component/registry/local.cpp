/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

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

//--- source -------------------------------------------------------------------
	source::source( const std::string& path )
		: library( path ), module( library.find< ooe::module ( void ) >( "module_open" )() )
	{
	}

	const ooe::module& source::get( void ) const
	{
		return module;
	}

//--- local --------------------------------------------------------------------
	local::local( const std::string& path )
		: source( path ), vector( static_cast< const facade::local* >
			( source.get().find( typeid( facade::local ).name() ) )->get() )
	{
	}

	bool local::supports( const interface& interface ) const
	{
		const module::vector_type& names = source.get().get();
		const interface::vector_type& faces = interface.get();

		interface::vector_type::const_iterator fend = faces.end();
		interface::vector_type::const_iterator f = faces.begin();
		module::vector_type::const_iterator nend = names.end();
		module::vector_type::const_iterator n = std::lower_bound( names.begin(), nend, *f );

		for ( ; n != nend && f != fend; ++n )
		{
			if ( *n == *f )
				++f;
		}

		return f == fend;
	}

	any local::find( const std::string& name, const std::string& type ) const
	{
		module::vector_tuple tuple( name, type );
		const module::vector_type& names = source.get().get();
		module::vector_type::const_iterator end = names.end();
		module::vector_type::const_iterator i = std::lower_bound( names.begin(), end, tuple );

		if ( i == end )
			throw error::runtime( "local: " ) <<
				"Unable to find \"" << name << "\" of type \"" << type << "\", does not exist";

		facade::local::vector_type::const_iterator j = vector.begin();
		std::advance( j, std::distance( names.begin(), i ) );
		return *j;
	}
}
