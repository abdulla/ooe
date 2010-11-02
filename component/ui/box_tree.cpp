/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cmath>

#include "component/ui/box_tree.hpp"
#include "foundation/math/geometry.hpp"
#include "foundation/utility/binary.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

geometry::intersection includes( const box& a, const box& b )
{
    if ( a.x >= b.x + b.width || a.x + a.width <= b.x ||
        a.y >= b.y + b.height || a.y + a.height <= b.y )
        return geometry::outside;
    else if ( a.x <= b.x && a.x + a.width >= b.x + b.width &&
        a.y <= b.y && a.y + a.height >= b.y + b.height )
        return geometry::inside;
    else
        return geometry::intersect;
}

box_tree* find_root( box_tree* root, unit& x, unit& y, u16 z )
{
    box box = root->box();

    if ( box.x > x.integer || box.y > y.integer ||
        box.x + box.width < x.integer || box.y + box.height < y.integer )
        return 0;

    x.fraction *= 2;
    y.fraction *= 2;
    x.integer = ( x.integer - box.x ) * 2 + x.fraction;
    y.integer = ( y.integer - box.y ) * 2 + y.fraction;

    if ( !z )
        return root;

    for ( box_tree::iterator i = root->begin(), end = root->end(); i != end; ++i )
    {
        if ( ( root = find_root( &*i, x, y, z - 1 ) ) )
            return root;
    }

    return 0;
}

bool find_view( const box_tree& tree, box_tree::box_vector& vector, u16 width, u16 height,
    unit x, unit y, u16 z, u8 level, u8 level_limit )
{
    if ( level == level_limit )
        return false;

    box view( width >> z, height >> z, x.integer, y.integer );
    box box = tree.box();

    if ( includes( view, box ) == geometry::outside )
        return false;

    u16 w = box.width;
    u16 h = box.height;
    x.integer = box.x < x.integer ? x.integer - box.x : 0;
    y.integer = box.y < y.integer ? y.integer - box.y : 0;
    x.integer = saturated_shift( x.integer );
    y.integer = saturated_shift( y.integer );

    if ( z )
    {
        w <<= z;
        h <<= z;
        x.fraction += box.x * exp2f( z );
        y.fraction += box.y * exp2f( z );

        for ( box_tree::const_iterator j = tree.begin(), end = tree.end(); j != end; ++j )
        {
            if ( find_view( *j, vector, width, height, x, y, z - 1, level, level_limit ) )
                return true;
        }
    }
    else
    {
        w >>= level;
        h >>= level;

        if ( !w || !h )
            return false;

        x.fraction += box.x / exp2f( level );
        y.fraction += box.y / exp2f( level );
        width = saturated_shift( width );
        height = saturated_shift( width );

        for ( box_tree::const_iterator j = tree.begin(), end = tree.end(); j != end; ++j )
        {
            if ( find_view( *j, vector, width, height, x, y, 0, level + 1, level_limit ) )
                return true;
        }
    }

    vector.push_back( make_tuple( w, h, x.fraction, y.fraction, -z + level ) );
    return includes( box, view ) == geometry::inside;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- unit -----------------------------------------------------------------------------------------
unit::unit( u16 integer_, f32 fraction_ )
    : integer( integer_ ), fraction( fraction_ )
{
    if ( fraction >= 1 )
        throw error::runtime( "unit: " ) << "Fraction " << fraction << " >= 1";
}

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

box box_tree::box( void ) const
{
    return bound;
}

box_tree::iterator box_tree::begin( void )
{
    return children.begin();
}

box_tree::iterator box_tree::end( void )
{
    return children.end();
}

box_tree::const_iterator box_tree::begin( void ) const
{
    return children.begin();
}

box_tree::const_iterator box_tree::end( void ) const
{
    return children.end();
}

box_tree::iterator box_tree::insert( u16 width, u16 height, u16 x, u16 y )
{
    iterator back = end();

    if ( !width || !height )
        return back;

    ooe::box b( width, height, x, y );
    ooe::box c( bound.width * 2, bound.height * 2, 0, 0 );

    if ( ::includes( c, b ) != geometry::inside )
        return back;

    for ( iterator i = begin(); i != back; ++i )
    {
        if ( ::includes( i->bound, b ) != geometry::outside )
            return back;
    }

    children.push_back( b );
    return --end();
}

box_tree::iterator box_tree::insert( u16 width, u16 height, unit x, unit y, u16 z )
{
    x.integer += bound.x;
    y.integer += bound.y;
    box_tree* root = find_root( this, x, y, z );
    return root ? root->insert( width, height, x.integer, y.integer ) : end();
}

box_tree::box_vector box_tree::view( u16 width, u16 height, unit x, unit y, u16 z ) const
{
    u16 level_limit = std::min( log2f( width ), log2f( height ) );
    f32 multiplier = -( 1 << z );
    x.fraction = ( x.integer + x.fraction ) * multiplier;
    y.fraction = ( y.integer + y.fraction ) * multiplier;

    box_vector vector;
    find_view( *this, vector, width, height, x, y, z, 0, level_limit );
    return vector;
}

OOE_NAMESPACE_END( ( ooe ) )
