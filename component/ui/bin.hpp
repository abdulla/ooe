/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_BIN_HPP
#define OOE_COMPONENT_UI_BIN_HPP

#include "foundation/utility/pointer.hpp"
#include "foundation/utility/tuple.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- rect -----------------------------------------------------------------------------------------
struct rect
{
    u32 x;
    u32 y;
    u32 width;
    u32 height;

    rect( u32, u32, u32, u32 );
};

//--- bin_node -------------------------------------------------------------------------------------
struct bin_node
{
    typedef scoped_ptr< bin_node > node_type;

    ooe::rect rect;
    node_type left;
    node_type right;

    bin_node( const ooe::rect& );
};

//--- bin ------------------------------------------------------------------------------------------
class bin
{
public:
    typedef tuple< u32, u32, bool > insert_type;

    bin( u32, u32 );

    insert_type insert( u32, u32 );
    void clear( void );

private:
    bin_node node;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_COMPONENT_UI_BIN_HPP
