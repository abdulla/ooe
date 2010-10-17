/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cmath>

#include "component/ui/box_tree.hpp"
#include "foundation/math/geometry.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

typedef box_tree::point_vector::const_iterator point_iterator;

geometry::intersection includes( const box& a, const box& b )
{
    if ( a.x > b.x + b.width || a.x + a.width < b.x ||
        a.y > b.y + b.height || a.y + a.height < b.y )
        return geometry::outside;
    else if ( a.x < b.x && a.x + a.width > b.x + b.width &&
        a.y < b.y && a.y + a.height > b.y + b.height )
        return geometry::inside;
    else
        return geometry::intersect;
}

geometry::intersection includes( const box& a, const point& b )
{
    if ( a.x > b.x || a.y > b.y || a.x + a.width <= b.x || a.y + a.height <= b.y )
        return geometry::outside;
    else
        return geometry::inside;
}

const box_tree& find_root( const box_tree& tree, point_iterator i, point_iterator i_end )
{
    const box_tree* root = &tree;

    for ( ; i != i_end; ++i )
    {
        box_tree::iterator j = root->begin();
        box_tree::iterator j_end = root->end();

        for ( ; j != j_end; ++j )
        {
            if ( ::includes( j->box(), *i ) == geometry::outside )
                continue;

            root = &*j;
            break;
        }

        if ( j == j_end )
            break;
    }

    return *root;
}

void find_view( const box_tree& tree, box_tree::view_vector& vector, u8 level, u8 level_limit )
{
    if ( level != level_limit )
    {
        for ( box_tree::iterator i = tree.begin(), end = tree.end(); i != end; ++i )
            find_view( tree, vector, level + 1, level_limit );
    }

    ooe::box box = tree.box();

    if ( ( box.width >> level ) && ( box.height >> level ) )
        vector.push_back( make_tuple( level, box ) );
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- point ----------------------------------------------------------------------------------------
point::point( u16 x_, u16 y_ )
    : x( x_ ), y( y_ )
{
}

//--- box ------------------------------------------------------------------------------------------
box::box( u16 x_, u16 y_, u16 width_, u16 height_ )
    : x( x_ ), y( y_ ), width( width_ ), height( height_ )
{
}

box scale_up( const box& in, u16 value )
{
    return box( in.x << value, in.y << value, in.width << value, in.height << value );
}

box scale_down( const box& in, u16 value )
{
    return box( in.x >> value, in.y >> value, in.width >> value, in.height >> value );
}

//--- box_tree -------------------------------------------------------------------------------------
box_tree::box_tree( const ooe::box& bound_ )
    : bound( bound_ ), children()
{
}

box_tree::iterator box_tree::begin( void ) const
{
    return children.begin();
}

box_tree::iterator box_tree::end( void ) const
{
    return children.end();
}

box box_tree::box( void ) const
{
    return bound;
}

bool box_tree::insert( const point_vector& points, const ooe::box& in )
{
    const box_tree& root = find_root( *this, points.begin(), points.end() );

    if ( ::includes( scale_up( root.bound, 1 ), in ) != geometry::inside )
        return false;

    for ( iterator i = begin(), i_end = end(); i != i_end; ++i )
    {
        if ( ::includes( i->bound, in ) != geometry::outside )
            return false;
    }

    children.push_back( in );
    return true;
}

box_tree::view_vector box_tree::
    view( const point_vector& points, const ooe::box& in, u8 level_limit ) const
{
    const box_tree& root = find_root( *this, points.begin(), points.end() );
    view_vector vector( 1, make_tuple( 0, root.bound ) );

    for ( box_tree::iterator i = begin(), i_end = end(); i != i_end; ++i )
    {
        if ( ::includes( i->bound, in ) != geometry::outside )
            find_view( *i, vector, 1, level_limit );
    }

    return vector;
}

OOE_NAMESPACE_END( ( ooe ) )
