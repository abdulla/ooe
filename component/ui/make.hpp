/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_MAKE_HPP
#define OOE_COMPONENT_UI_MAKE_HPP

#include "component/ui/box_tree.hpp"
#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

program_type make_program( const device_type&, const std::string&, const std::string& );
buffer_type make_index( const device_type& );
buffer_type make_point( const device_type& );
box_tree make_tree( const std::string& );

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_COMPONENT_UI_MAKE_HPP
