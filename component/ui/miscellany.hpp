/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_MISCELLANY_HPP
#define OOE_COMPONENT_UI_MISCELLANY_HPP

#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- colour ---------------------------------------------------------------------------------------
struct colour
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
    {
        map_ptr map = index->map( buffer::write );
        u16* value = static_cast< u16* >( map->data );

        value[ 0 ] = 0;
        value[ 1 ] = 1;
        value[ 2 ] = 2;
        value[ 3 ] = 2;
        value[ 4 ] = 1;
        value[ 5 ] = 3;
    }
    return index;
}

//--- make_point -----------------------------------------------------------------------------------
inline buffer_ptr make_point( const device_ptr& device )
{
    buffer_ptr point = device->buffer( sizeof( f32 ) * 2 * 4, buffer::point );
    {
        map_ptr map = point->map( buffer::write );
        f32* value = static_cast< f32* >( map->data );

        // top left
        value[ 0 ] = 0;
        value[ 1 ] = 1;

        // bottom left
        value[ 2 ] = 0;
        value[ 3 ] = 0;

        // top right
        value[ 4 ] = 1;
        value[ 5 ] = 1;

        // bottom right
        value[ 6 ] = 1;
        value[ 7 ] = 0;
    }
    return point;
}

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_COMPONENT_UI_MISCELLANY_HPP
