/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "component/ui/text_layout.hpp"
#include "foundation/utility/arithmetic.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

const up_t point_size = 8;

u32 add_glyph
    ( const font_source::glyph_type& glyph, f32* value, u32 size, s32 x, s32 y, u32 max, u8 level )
{
    const font::metric& metric = glyph._2;

    value[ 0 ] = metric.width;
    value[ 1 ] = metric.height;
    value[ 2 ] = x;
    value[ 3 ] = y + ( max - metric.top );

    value[ 4 ] = divide( metric.width << level, size );
    value[ 5 ] = divide( metric.height << level, size );
    value[ 6 ] = divide( glyph._0, size );
    value[ 7 ] = divide( glyph._1, size );

    return metric.width + metric.left;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- text_layout ----------------------------------------------------------------------------------
text_layout::
    text_layout( const device_type& device_, virtual_texture& vt_, const font_source& source_ )
    : device( device_ ), vt( vt_ ), source( source_ )
{
}

u32 text_layout::input( const block_type& block, const std::string& text, u8 level )
{
    buffer_type point = device->buffer( sizeof( f32 ) * point_size * text.size(), buffer::point );

    if ( !text.empty() )
    {
        map_type map = point->map( buffer::write );
        f32* value = static_cast< f32* >( map->data );
        u32 size = source.size();
        s32 x = 0;
        s32 y = 0;
        u32 max = source.font_size() >> level;

        for ( std::string::const_iterator i = text.begin(), end = text.end(); i != end;
            ++i, value += point_size )
        {
            font_source::glyph_type glyph = source.glyph( u8( *i ), level );
            x += add_glyph( glyph, value, size, x, y, max, level );
            vt.load( glyph._0, glyph._1, glyph._2.width, glyph._2.height, level );
        }
    }

    block->input( "vertex_scale", 2, point, true );
    block->input( "vertex_translate", 2, point, true );
    block->input( "coord_scale", 2, point, true );
    block->input( "coord_translate", 2, point, true );
    return text.size();
}

OOE_NAMESPACE_END( ( ooe ) )
