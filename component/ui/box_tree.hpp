/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_BOX_TREE_HPP
#define OOE_COMPONENT_UI_BOX_TREE_HPP

#include <vector>

#include "foundation/utility/tuple.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- point ----------------------------------------------------------------------------------------
struct point
{
    u16 x;
    u16 y;

    point( u16, u16 );
};

//--- box ------------------------------------------------------------------------------------------
struct box
{
    u16 width;
    u16 height;
    u16 x;
    u16 y;

    box( u16, u16, u16, u16 );
};

box scale_up( const box&, u16 );
box scale_down( const box&, u16 );

//--- box_tree -------------------------------------------------------------------------------------
class box_tree
{
public:
    typedef std::vector< box_tree > node_vector;
    typedef node_vector::const_iterator iterator;
    typedef tuple< f32, f32, f32, f32, f32 > box_tuple; // width, height, x, y, z
    typedef std::vector< box_tuple > box_vector;
    typedef std::vector< point > point_vector;

    box_tree( const ooe::box& );

    iterator begin( void ) const;
    iterator end( void ) const;
    ooe::box box( void ) const;

    bool insert( const point_vector&, const ooe::box& );
    box_vector view( const point_vector&, const ooe::box&, u8 ) const;

private:
    ooe::box bound;
    node_vector children;
};

OOE_NAMESPACE_END( ( ooe ) )


#endif  // OOE_COMPONENT_UI_BOX_TREE_HPP
