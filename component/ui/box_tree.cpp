/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "component/ui/box_tree.hpp"
#include "foundation/math/geometry.hpp"
#include "foundation/utility/arithmetic.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

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

box intersection( const box& a, const box& b )
{
    u16 x = std::max( a.x, b.x );
    u16 y = std::max( a.y, b.y );
    u16 w = std::min( a.width + a.x, b.width + b.x ) - x;
    u16 h = std::min( a.height + a.y, b.height + b.y ) - y;
    return box( w, h, x - a.x, y - a.y );
}

box_tree* find_root( box_tree* root, fx1664& x, fx1664& y )
{
    box box = root->box();

    if ( box.x > x.magnitude() || box.y > y.magnitude() ||
        box.x + box.width < x.magnitude() || box.y + box.height < y.magnitude() )
        return 0;

    x -= box.x;
    y -= box.y;
    x <<= 1;
    y <<= 1;

    for ( box_tree::iterator i = root->begin(), end = root->end(); i != end; ++i )
    {
        if ( ( root = find_root( &*i, x, y ) ) )
            return root;
    }

    return root;
}

void find_view( const box_tree& tree, box_tree::layer_vector& layer, f32 begin_x, f32 begin_y,
    u16 width, u16 height, fx1664 x, fx1664 y, s16 z, u16 i, u16 i_end )
{
    if ( i == i_end )
        return;

    s16 slide = z - i;
    u16 view_w = saturated_slide( width, -slide ) + 1;
    u16 view_h = saturated_slide( height, -slide ) + 1;
    box view( view_w ? view_w : 0xffff, view_h ? view_h : 0xffff, x.magnitude(), y.magnitude() );
    box box = tree.box();
    geometry::intersection result = includes( view, box );

    if ( result == geometry::outside )
        return;

    u32 out_w = bit_slide< u32 >( box.width, slide );
    u32 out_h = bit_slide< u32 >( box.height, slide );

    if ( !out_w || !out_h )
        return;

    f32 out_x = begin_x + bit_slide< u32 >( box.x, slide );
    f32 out_y = begin_y + bit_slide< u32 >( box.y, slide );
    box_tree::data_tuple out( out_w, out_h, out_x, out_y );
    box_tree::data_tuple in( 1, 1, 0, 0 );

    if ( result == geometry::intersect )
    {
        ooe::box intersect = intersection( box, view );
        f32 in_w = divide( intersect.width, box.width );
        f32 in_h = divide( intersect.height, box.height );
        f32 in_x = divide( intersect.x, box.width );
        f32 in_y = divide( intersect.y, box.height );
        in = make_tuple( in_w, in_h, in_x, in_y );
        width = bit_slide( intersect.width, slide );
        height = bit_slide( intersect.height, slide );
    }

    layer.resize( i < layer.size() ? layer.size() : i + 1 );
    layer[ i ].push_back( make_tuple( out, in, tree.colour(), tree.get() ) );

    x = clamped_subtract( x, box.x );
    x = saturated_shift( x, 1 );
    y = clamped_subtract( y, box.y );
    y = saturated_shift( y, 1 );

    for ( box_tree::const_iterator j = tree.begin(), j_end = tree.end(); j != j_end; ++j )
        find_view( *j, layer, out_x, out_y, width, height, x, y, z, i + 1, i_end );
}

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- box ------------------------------------------------------------------------------------------
box::box( u16 width_, u16 height_, u16 x_, u16 y_ )
    : width( width_ ), height( height_ ), x( x_ ), y( y_ )
{
}

//--- box_tree -------------------------------------------------------------------------------------
box_tree::box_tree( const ooe::box& bound_, const ooe::colour& hue_, const opaque_ptr& pointer_ )
    : bound( bound_ ), hue( hue_ ), pointer( pointer_ ), children()
{
}

box box_tree::box( void ) const
{
    return bound;
}

colour box_tree::colour( void ) const
{
    return hue;
}

void* box_tree::get( void ) const
{
    return pointer;
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

box_tree::iterator box_tree::
    insert( const ooe::box& bound_, const ooe::colour& hue_, const opaque_ptr& pointer_ )
{
    iterator back = end();

    if ( !bound_.width || !bound_.height )
        return back;

    ooe::box container( bound.width * 2, bound.height * 2, 0, 0 );

    if ( ::includes( container, bound_ ) != geometry::inside )
        return back;

    for ( iterator i = begin(); i != back; ++i )
    {
        if ( ::includes( i->bound, bound_ ) != geometry::outside )
            return back;
    }

    return children.insert( back, box_tree( bound_, hue_, pointer_ ) );
}

box_tree::find_tuple box_tree::find( fx1664 x, fx1664 y )
{
    if ( x.magnitude() < bound.x || y.magnitude() < bound.y )
        return find_tuple( *this, false );

    x -= bound.x;
    y -= bound.y;
    box_tree* root = find_root( this, x, y );
    return find_tuple( root ? *root : *this, true );
}

box_tree::layer_vector box_tree::view( u16 width, u16 height, fx1664 x, fx1664 y, s16 z ) const
{
    f32 multiplier = -exp2f( z );
    f32 begin_x = x.floating_point() * multiplier;
    f32 begin_y = y.floating_point() * multiplier;
    u16 level_limit = std::min( log2f( width ), log2f( height ) );
    layer_vector layer;
    find_view( *this, layer, begin_x, begin_y, width, height, x, y, z, 0, z + level_limit );
    return layer;
}

OOE_NAMESPACE_END( ( ooe ) )
