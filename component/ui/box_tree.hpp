/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_BOX_TREE_HPP
#define OOE_COMPONENT_UI_BOX_TREE_HPP

#include <vector>

#include "foundation/utility/tuple.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- box_unit -------------------------------------------------------------------------------------
struct box_unit
{
    u16 integer;
    f32 fraction;

    box_unit( u16, f32 );
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

//--- box_tree -------------------------------------------------------------------------------------
class box_tree
{
public:
    typedef std::vector< box_tree > node_vector;
    typedef node_vector::iterator iterator;
    typedef node_vector::const_iterator const_iterator;
    typedef tuple< f32, f32, f32, f32, f32 > box_tuple; // width, height, x, y, z
    typedef std::vector< box_tuple > box_vector;

    box_tree( const ooe::box& );
    ooe::box box( void ) const;

    iterator begin( void );
    iterator end( void );
    const_iterator begin( void ) const;
    const_iterator end( void ) const;

    iterator insert( u16, u16, u16, u16 );
    iterator insert( u16, u16, box_unit, box_unit, u16 );
    box_vector view( u16, u16, box_unit, box_unit, u16 ) const;

private:
    ooe::box bound;
    node_vector children;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_COMPONENT_UI_BOX_TREE_HPP
