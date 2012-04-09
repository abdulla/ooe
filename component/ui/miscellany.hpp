/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_MISCELLANY_HPP
#define OOE_COMPONENT_UI_MISCELLANY_HPP

#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- colour ---------------------------------------------------------------------------------------
struct OOE_PACKED colour
{
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;

    colour( u8 red_, u8 green_, u8 blue_, u8 alpha_ )
        : red( red_ ), green( green_ ), blue( blue_ ), alpha( alpha_ )
    {
    }
};

//--- make_index -----------------------------------------------------------------------------------
inline buffer_ptr make_index( const device_ptr& device )
{
    buffer_ptr index = device->buffer( sizeof( u16 ) * 6, buffer::index );
    checked_map map = index->map( buffer::write );
    u16 value[] = { 0, 1, 2, 2, 1, 3 };
    map.write( value, sizeof( value ) );
    return index;
}

//--- make_point -----------------------------------------------------------------------------------
inline buffer_ptr make_point( const device_ptr& device )
{
    buffer_ptr point = device->buffer( sizeof( f32 ) * 2 * 4, buffer::point );
    checked_map map = point->map( buffer::write );
    f32 value[] =
    {
        // top left
        0, 1,
        // bottom left
        0, 0,
        // top right
        1, 1,
        // bottom right
        1, 0
    };
    map.write( value, sizeof( value ) );
    return point;
}

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_COMPONENT_UI_MISCELLANY_HPP
