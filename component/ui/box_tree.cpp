/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cmath>

#include "component/ui/box_tree.hpp"
#include "foundation/math/geometry.hpp"
#include "foundation/utility/binary.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

geometry::intersection includes( const box& a, const box& b )
{
    if ( a.x >= b.x + b.width || a.x + a.width <= b.x ||
        a.y >= b.y + b.height || a.y + a.height <= b.y )
        return geometry::outside;
    else if ( a.x < b.x && a.x + a.width > b.x + b.width &&
        a.y < b.y && a.y + a.height > b.y + b.height )
        return geometry::inside;
    else
        return geometry::intersect;
}

const box_tree* find_root( const box_tree* root, u16 a, u16 b, f32 x, f32 y, u16 z )
{
    for ( u16 i = 0; i != z; ++i )
    {
        box_tree::iterator j = root->begin();
        box_tree::iterator end = root->end();

        for ( ; j != end; ++j )
        {
            box box = j->box();

            if ( box.x > a || box.y > b || box.x + box.width < a || box.y + box.height < b )
                continue;

            x *= 2;
            y *= 2;
            a = a - box.x + x;
            b = b - box.y + y;
            x -= 1;
            y -= 1;
            root = &*j;
            break;
        }

        if ( j == end )
            return 0;
    }

    return root;
}

void find_view( const box_tree& tree, box_tree::box_vector& vector, box in, f32 x, f32 y, u16 z,
    u8 level, u8 level_limit )
{
    if ( level == level_limit )
        return;

    box view( in.width >> z, in.height >> z, in.x, in.y );
    box box = tree.box();

    if ( includes( view, box ) == geometry::outside )
        return;

    u16 w = box.width;
    u16 h = box.height;
    in.x = box.x < in.x ? in.x - box.x : 0;
    in.y = box.y < in.y ? in.y - box.y : 0;
    in.x = saturated_shift( in.x );
    in.y = saturated_shift( in.y );

    if ( z )
    {
        w <<= z;
        h <<= z;
        x += box.x << z;
        y += box.y << z;

        for ( box_tree::iterator j = tree.begin(), end = tree.end(); j != end; ++j )
            find_view( *j, vector, in, x, y, z - 1, level, level_limit );
    }
    else
    {
        w >>= level;
        h >>= level;
        x += box.x / f32( 1 << level );
        y += box.y / f32( 1 << level );
        in.width = saturated_shift( in.width );
        in.height = saturated_shift( in.width );

        for ( box_tree::iterator j = tree.begin(), end = tree.end(); j != end; ++j )
            find_view( *j, vector, in, x, y, 0, level + 1, level_limit );
    }

    if ( w && h )
        vector.push_back( make_tuple( w, h, x, y, -z + level ) );
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- box ------------------------------------------------------------------------------------------
box::box( u16 width_, u16 height_, u16 x_, u16 y_ )
    : width( width_ ), height( height_ ), x( x_ ), y( y_ )
{
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

bool box_tree::insert( const ooe::box& in, f32 x, f32 y, u16 z )
{
    const box_tree* root = find_root( this, in.x, in.y, x, y, z );

    if ( !root )
        return false;

    ooe::box b( root->bound.width * 2, root->bound.height * 2, 0, 0 );

    if ( ::includes( b, in ) != geometry::inside )
        return false;

    for ( iterator i = root->begin(), i_end = root->end(); i != i_end; ++i )
    {
        if ( ::includes( i->bound, in ) != geometry::outside )
            return false;
    }

    const_cast< node_vector& >( root->children ).push_back( in );
    return true;
}

box_tree::box_vector box_tree::view( const ooe::box& in, f32 x, f32 y, u16 z ) const
{
    u16 level_limit = std::min( log2f( in.width ), log2f( in.height ) );
    box_vector vector;
    f32 m = 1 << z;
    find_view( *this, vector, in, ( -x - in.x ) * m, ( -y - in.y ) * m, z, 0, level_limit );
    return vector;
}

OOE_NAMESPACE_END( ( ooe ) )
