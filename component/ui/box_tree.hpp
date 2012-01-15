/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_BOX_TREE_HPP
#define OOE_COMPONENT_UI_BOX_TREE_HPP

#include "component/ui/miscellany.hpp"
#include "foundation/utility/fixed_point.hpp"
#include "foundation/utility/tuple.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

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
    typedef std::vector< box_tree > tree_vector;
    typedef tree_vector::iterator iterator;
    typedef tree_vector::const_iterator const_iterator;
    typedef tuple< box_tree&, bool > find_tuple;
    typedef tuple< f32 /* width */, f32 /* height */, f32 /* x */, f32 /* y */ > data_tuple;
    typedef std::vector< tuple< data_tuple, data_tuple, ooe::colour, void* > > data_vector;
    typedef std::vector< data_vector > layer_vector;

    box_tree( const ooe::box&, const ooe::colour&, const opaque_ptr& );
    ooe::box box( void ) const;
    ooe::colour colour( void ) const;
    void* get( void ) const;

    iterator begin( void );
    iterator end( void );
    const_iterator begin( void ) const;
    const_iterator end( void ) const;

    iterator insert( const ooe::box&, const ooe::colour&, const opaque_ptr& );
    find_tuple find( fx1664, fx1664 );
    layer_vector view( u16, u16, fx1664, fx1664, s16 ) const;

private:
    ooe::box bound;
    ooe::colour hue;
    opaque_ptr pointer;
    tree_vector children;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_COMPONENT_UI_BOX_TREE_HPP
