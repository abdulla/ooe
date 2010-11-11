/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_MAKE_HPP
#define OOE_COMPONENT_UI_MAKE_HPP

#include <map>

#include "component/ui/node.hpp"
#include "foundation/utility/function.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

typedef std::map< std::string, function< node* ( const std::string& ) > > node_map;

program_type make_program( const device_type&, const std::string&, const std::string& );
buffer_type make_index( const device_type& );
buffer_type make_point( const device_type& );
box_tree make_tree( const std::string&, const node_map& );

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_COMPONENT_UI_MAKE_HPP
