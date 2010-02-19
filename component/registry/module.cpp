/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "component/registry/module.hpp"
#include "foundation/utility/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- facade_id ------------------------------------------------------------------------------------
facade_id::facade_id( const void* pointer_, function_type function_ )
	: pointer( pointer_ ), function( function_ )
{
}

facade_id::~facade_id( void )
{
	function( pointer );
}

//--- module ---------------------------------------------------------------------------------------
const interface::vector_type& module::names( void ) const
{
	return face.get();
}

const module::vector_type& module::docs( void ) const
{
	return vector;
}

up_t module::insert( const std::string& name, const std::string& type, const c8* doc )
{
	up_t index = face.insert( name, type );
	vector_type::iterator i = vector.begin();
	std::advance( i, index );
	vector.insert( i, doc );
	return index;
}

const void* module::find( const std::string& name ) const
{
	map_type::const_iterator i = map.find( name );

	if ( i == map.end() || i->first != name )
		throw error::runtime( "module: " ) << "Unable to find \"" << name << "\", does not exist";

	return i->second->pointer;
}

void module::insert( const std::string& name, const map_ptr& pointer )
{
	map_type::const_iterator i = map.find( name );

	if ( i != map.end() )
		throw error::runtime( "module: " ) << "Unable to insert \"" << name << "\", facade exists";

	map.insert( map_type::value_type( name, pointer ) );
}

OOE_NAMESPACE_END( ( ooe ) )
