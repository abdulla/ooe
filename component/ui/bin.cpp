/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "component/ui/bin.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

bin_node* find_root( bin_node* node, u32 width, u32 height )
{
    if ( !node->left )
        return width > node->rect.width || height > node->rect.height ? 0 : node;
    else if ( !node->right->rect.width || !node->right->rect.height )
        return 0;
    else if ( ( node = find_root( node->left, width, height ) ) )
        return node;
    else
        return find_root( node->right, width, height );
}

void split_node( bin_node& node, u32 width, u32 height )
{
    const rect& r = node.rect;
    rect a( r.x, r.y, r.width, r.height );
    rect b( r.x + width, r.y + height, r.width - width, r.height - height );
    node.left.reset( new bin_node( a ) );
    node.right.reset( new bin_node( b ) );
}

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- rect -----------------------------------------------------------------------------------------
rect::rect( u32 x_, u32 y_, u32 width_, u32 height_ )
    : x( x_ ), y( y_ ), width( width_ ), height( height_ )
{
}

//--- bin_node -------------------------------------------------------------------------------------
bin_node::bin_node( const ooe::rect& rect_ )
    : rect( rect_ ), left( 0 ), right( 0 )
{
}

//--- bin ------------------------------------------------------------------------------------------
bin::bin( u32 width, u32 height )
    : node( rect( 0, 0, width, height ) )
{
}

bin::insert_type bin::insert( u32 width, u32 height )
{
    bin_node* root = find_root( &node, width, height );

    if ( !root )
        return insert_type( 0, 0, false );

    const rect& rect = root->rect;
    u32 wide = rect.width - width;
    u32 tall = rect.height - height;

    if ( wide >= tall )
        split_node( *root, width, 0 );
    else
        split_node( *root, 0, height );

    return insert_type( rect.x, rect.y, true );
}

void bin::clear( void )
{
    node.left.reset();
    node.right.reset();
}

OOE_NAMESPACE_END( ( ooe ) )
