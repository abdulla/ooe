/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "component/ui/text_layout.hpp"
#include "foundation/utility/arithmetic.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

const up_t index_size = 6;
const up_t point_size = 4 * 4;

u32 add_glyph
    ( const font_source::glyph_type& glyph, f32* value, u32 size, s32 x, s32 y, u32 max, u8 level )
{
    const font::metric& m = glyph._2;

    f32 x_min = x + m.left;
    f32 x_max = x + m.width + m.left;
    f32 y_min = y + ( max - m.top );
    f32 y_max = y + m.height + ( max - m.top );

    f32 u_min = divide( glyph._0, size );
    f32 u_max = divide( glyph._0 + ( m.width << level ), size );
    f32 v_min = divide( glyph._1, size );
    f32 v_max = divide( glyph._1 + ( m.height << level ), size );

    // top left
    value[ 0 ] = x_min;
    value[ 1 ] = y_max;
    value[ 2 ] = u_min;
    value[ 3 ] = v_max;

    // bottom left
    value[ 4 ] = x_min;
    value[ 5 ] = y_min;
    value[ 6 ] = u_min;
    value[ 7 ] = v_min;

    // top right
    value[ 8 ] = x_max;
    value[ 9 ] = y_max;
    value[ 10 ] = u_max;
    value[ 11 ] = v_max;

    // bottom right
    value[ 12 ] = x_max;
    value[ 13 ] = y_min;
    value[ 14 ] = u_max;
    value[ 15 ] = v_min;

    return m.width + m.left;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- text_layout ----------------------------------------------------------------------------------
text_layout::
    text_layout( const device_type& device_, virtual_texture& vt_, const font_source& source_ )
    : device( device_ ), vt( vt_ ), source( source_ )
{
}

block_type text_layout::block( const program_type& program, const std::string& text, u8 level )
{
    buffer_type index = device->buffer( sizeof( u16 ) * index_size * text.size(), buffer::index );

    if ( !text.empty() )
    {
        map_type map = index->map( buffer::write );
        u16* value = static_cast< u16* >( map->data );

        for ( up_t i = 0, end = text.size() * 4; i != end; i += 4, value += index_size )
        {
            value[ 0 ] = 0 + i;
            value[ 1 ] = 1 + i;
            value[ 2 ] = 2 + i;
            value[ 3 ] = 2 + i;
            value[ 4 ] = 1 + i;
            value[ 5 ] = 3 + i;
        }
    }

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

    block_type b = program->block( index );
    b->input( "vertex", 2, point );
    b->input( "coords", 2, point );
    return b;
}

OOE_NAMESPACE_END( ( ooe ) )
