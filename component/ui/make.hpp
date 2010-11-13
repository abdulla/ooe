/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_MAKE_HPP
#define OOE_COMPONENT_UI_MAKE_HPP

#include <map>

#include "component/ui/box_tree.hpp"
#include "foundation/utility/function.hpp"
#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

struct node
{
    typedef tuple< block_type, u32 > block_tuple;

    virtual ~node( void ) {}
    virtual block_tuple block( const box_tree::box_tuple&, const box_tree::aux_tuple& ) = 0;
};

typedef std::map< std::string, function< node* ( const std::string& ) > > node_map;

program_type make_program( const device_type&, const std::string&, const std::string& );
buffer_type make_index( const device_type& );
buffer_type make_point( const device_type& );
box_tree make_tree( const std::string&, const node_map& );

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_COMPONENT_UI_MAKE_HPP
