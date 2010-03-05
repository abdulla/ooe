/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_REGISTRY_MODULE_HPP
#define OOE_COMPONENT_REGISTRY_MODULE_HPP

#include <map>

#include "component/registry/interface.hpp"
#include "foundation/utility/miscellany.hpp"
#include "foundation/utility/pointer.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- module ---------------------------------------------------------------------------------------
class module
{
public:
	typedef std::vector< const c8* > vector_type;
	typedef std::map< std::string, opaque_ptr > map_type;

	const interface::vector_type& names( void ) const OOE_VISIBLE;
	const vector_type& docs( void ) const OOE_VISIBLE;
	const c8* doc( const std::string&, const std::string& ) const OOE_VISIBLE;
	up_t insert( const std::string&, const std::string&, const c8* ) OOE_VISIBLE;

	const void* find( const std::string& ) const OOE_VISIBLE;
	void insert( const std::string&, const opaque_ptr& ) OOE_VISIBLE;

private:
	interface face;
	vector_type vector;
	map_type map;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_COMPONENT_REGISTRY_MODULE_HPP
