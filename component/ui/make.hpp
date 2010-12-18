/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_MAKE_HPP
#define OOE_COMPONENT_UI_MAKE_HPP

#include <map>

#include <boost/property_tree/ptree.hpp>

#include "component/ui/box_tree.hpp"
#include "component/ui/miscellany.hpp"
#include "foundation/utility/function.hpp"
#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- node -----------------------------------------------------------------------------------------
struct node
{
    typedef tuple< block_ptr, u32 > block_tuple;

    virtual ~node( void ) {}
    virtual block_tuple block( const box_tree::box_tuple&, const box_tree::aux_tuple& ) = 0;
};

//--- colour_node ----------------------------------------------------------------------------------
struct colour_node
{
    ooe::colour colour;
    scoped_ptr< ooe::node > node;

    colour_node( ooe::colour colour_, ooe::node* node_ )
        : colour( colour_ ), node( node_ )
    {
    }
};

typedef tuple< colour, scoped_ptr< node > > node_tuple;
typedef boost::property_tree::ptree property_tree;
typedef std::map< std::string, function< node* ( const property_tree& ) > > node_map;

program_ptr make_program( const device_ptr&, const std::string&, const std::string& );
buffer_ptr make_index( const device_ptr& );
buffer_ptr make_point( const device_ptr& );
colour make_colour( const property_tree&, u8 );
box_tree make_tree( const std::string&, const node_map& );

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_COMPONENT_UI_MAKE_HPP
