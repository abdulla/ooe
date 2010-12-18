/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_MISCELLANY_HPP
#define OOE_COMPONENT_UI_MISCELLANY_HPP

OOE_NAMESPACE_BEGIN( ( ooe ) )

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

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_COMPONENT_UI_MISCELLANY_HPP
