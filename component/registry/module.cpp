/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <algorithm>

#include "component/registry/module.hpp"
#include "foundation/utility/error.hpp"

namespace
{
	using namespace ooe;

	const std::string& validate( const module::info_tuple& info )
	{
		if ( info._0 != module::library )
			throw error::runtime( "internal: " ) << "Info " << info << " does not name a library";

		return info._1;
	}
}

namespace ooe
{
//--- facade_id ----------------------------------------------------------------
	facade_id::facade_id( const void* pointer_, function_type function_ )
		: pointer( pointer_ ), function( function_ )
	{
	}

	facade_id::~facade_id( void )
	{
		function( pointer );
	}

//--- module -------------------------------------------------------------------
	module::module( type t, const std::string& s )
		: info_( t, s ), names_(), data_(), facades()
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

	up_t module::insert( const name_tuple& name, any any )
	{
		name_vector::iterator i = std::lower_bound( names_.begin(), names_.end(), name );

		if ( i != names_.end() && *i == name )
			throw error::runtime( "module: " ) << "Function " << name << " exists";

		up_t distance = std::distance( names_.begin(), i );
		names_.insert( i, name );

		datum_vector::iterator j = data_.begin();
		std::advance( j, distance );
		data_.insert( j, any );
		return distance;
	}

	const void* module::facade( const std::string& name ) const
	{
		facade_map::const_iterator i = facades.find( name );

		if ( i == facades.end() )
			throw error::runtime( "module: " ) << "Facade \"" << name << "\" does not exists";

		return i->second->pointer;
	}

	void module::insert( const std::string& name, const facade_ptr& pointer )
	{
		facade_map::const_iterator i = facades.find( name );

		if ( i != facades.end() )
			throw error::runtime( "module: " ) << "Facade \"" << name << "\" exists";

		facades.insert( facade_map::value_type( name, pointer ) );
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
			throw error::runtime( "interface: " ) << "Function " << name << " exists";

		names_.insert( i, name );
	}

//--- internal -----------------------------------------------------------------
	internal::internal( const module::info_tuple& info )
		: library( validate( info ) ),
		module( library.find< ooe::module ( * )( void ) >( "" ).function() )
	{
	}

	any internal::find( const module::name_tuple& name ) const
	{
		const module::name_vector& names = module.names();
		module::name_vector::const_iterator i =
			std::lower_bound( names.begin(), names.end(), name );

		if ( i == names.end() )
			throw error::runtime( "internal: " ) << "Unable to find " << name;

		const module::datum_vector& data = module.data();
		module::datum_vector::const_iterator j = data.begin();
		std::advance( j, std::distance( names.begin(), i ) );
		return *j;
	}
}
