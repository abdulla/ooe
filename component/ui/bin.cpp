/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "component/ui/bin.hpp"

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
bin::bin( u32 size )
    : node( rect( 0, 0, size, size ) )
{
}

OOE_NAMESPACE_END( ( ooe ) )
