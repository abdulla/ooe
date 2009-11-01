/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <algorithm>

#include "component/registry/module.hpp"
#include "foundation/utility/error.hpp"

namespace
{
	using namespace ooe;

	std::string validate_internal( const module::info_tuple& info )
	{
		if ( info._0 != module::library )
			throw error::runtime( "internal: " ) << "Info " << info << " does not name a library";

		return info._1;
	}
}

namespace ooe
{
//--- module -------------------------------------------------------------------
	module::module( type t, const std::string& s )
		: info_( t, s ), names_(), data_()
	{
	}

	const module::info_tuple& module::info( void ) const
	{
		return info_;
	}

	const module::name_vector& module::names( void ) const
	{
		return names_;
	}

	const module::datum_vector& module::data( void ) const
	{
		return data_;
	}

	void module::insert( const name_tuple& name, any any )
	{
		name_vector::iterator i = std::lower_bound( names_.begin(), names_.end(), name );

		if ( i != names_.end() && *i == name )
			throw error::runtime( "module: " ) << "Function " << name << " exists";

		names_.insert( i, name );

		datum_vector::iterator j = data_.begin();
		std::advance( j, std::distance( names_.begin(), i ) );
		data_.insert( j, any );
	}

//--- interface ----------------------------------------------------------------
	const module::name_vector& interface::names( void ) const
	{
		return names_;
	}

	void interface::insert( const module::name_tuple& name )
	{
		module::name_vector::iterator i = std::lower_bound( names_.begin(), names_.end(), name );

		if ( i != names_.end() && *i == name )
			throw error::runtime( "module: " ) << "Function " << name << " exists";

		names_.insert( i, name );
	}

//--- internal -----------------------------------------------------------------
	internal::internal( const module::info_tuple& info )
		: library( validate_internal( info ) ),
		module( library.find< ooe::module ( * )( void ) >( "" ).function() )
	{
	}

//--- external -----------------------------------------------------------------
}
