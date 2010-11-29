/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_TEXT_LAYOUT_HPP
#define OOE_COMPONENT_UI_TEXT_LAYOUT_HPP

#include "component/ui/font_source.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- text -----------------------------------------------------------------------------------------
struct text
{
    std::string data;

    u16 x;
    u16 y;
    u8 level;

    u8 red;
    u8 green;
    u8 blue;

    text( void );
};

typedef std::vector< text > text_vector;

//--- text_layout ----------------------------------------------------------------------------------
class text_layout
{
public:
    text_layout( const device_ptr&, page_cache&, font_source& );

    u32 input( block_ptr&, const text_vector&, f32 );

private:
    const device_ptr& device;
    const font_source& source;
    virtual_texture texture;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_COMPONENT_UI_TEXT_LAYOUT_HPP
