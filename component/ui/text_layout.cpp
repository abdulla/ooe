/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <utf8.h>

#include "component/ui/text_layout.hpp"
#include "foundation/utility/arithmetic.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

typedef std::string::const_iterator iterator_type;
const up_t point_size = 8;

struct marker
{
    iterator_type i;
    f32* data;
    u32 width;

    marker( iterator_type i_, f32* data_, u32 width_ )
        : i( i_ ), data( data_ ), width( width_ )
    {
    }
};

void add_glyph
    ( const font_source::glyph_type& glyph, f32* data, u32 size, s32 x, s32 y, u32 max, u8 level )
{
    const font::metric& metric = glyph._2;

    data[ 0 ] = metric.width;
    data[ 1 ] = metric.height;
    data[ 2 ] = x;
    data[ 3 ] = y + ( max - metric.top );

    data[ 4 ] = divide( metric.width << level, size );
    data[ 5 ] = divide( metric.height << level, size );
    data[ 6 ] = divide( glyph._0, size );
    data[ 7 ] = divide( glyph._1, size );
}

u32 glyph_size( iterator_type i, iterator_type end )
{
    u32 glyphs = 0;

    while ( i != end )
    {
        if ( !std::iswspace( utf8::next( i, end ) ) )
            ++glyphs;
    }

    return glyphs;
}

bool handle_space( u32 code_point, s32& x, s32& y, u32 max )
{
    switch ( code_point )
    {
    case ' ':
        x += max >> 1;
        return true;

    case '\n':
        x = 0;
        y += max;
        return true;

    default:
        return std::iswspace( code_point );
    }
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- text_layout ----------------------------------------------------------------------------------
text_layout::
    text_layout( const device_type& device_, virtual_texture& vt_, const font_source& source_ )
    : device( device_ ), vt( vt_ ), source( source_ )
{
}

u32 text_layout::input( const block_type& block, const std::string& text, u8 level, u32 width )
{
    u32 glyphs = glyph_size( text.begin(), text.end() );

    if ( !glyphs )
        return 0;

    buffer_type point = device->buffer( sizeof( f32 ) * point_size * glyphs, buffer::point );
    map_type map = point->map( buffer::write );
    f32* data = static_cast< f32* >( map->data );
    u32 size = source.size();
    s32 x = 0;
    s32 y = 0;
    u32 max = source.font_size() >> level;
    marker m( text.begin(), data, 0 );
    u32 code_point = 0;
    u32 last_point;

    for ( iterator_type i = m.i, end = text.end(); i != end; )
    {
        code_point = utf8::next( i, end );

        if ( handle_space( code_point, x, y, max ) )
        {
            m = marker( i, data, 0 );
            continue;
        }
        else if ( x )
        {
            font::kerning kerning = source.kerning( last_point, code_point, level );
            x += kerning.x;
        }

        font_source::glyph_type glyph = source.glyph( code_point, level );
        u32 x_offset = glyph._2.x;

        if ( x + x_offset > width )
        {
            x = 0;
            y += max;

            if ( m.width + x_offset > width )
                utf8::prior( i, text.begin() );
            else
            {
                i = m.i;
                data = m.data;
            }

            m.width = 0;
            continue;
        }

        vt.load( glyph._0, glyph._1, glyph._2.width, glyph._2.height, level );
        add_glyph( glyph, data, size, x, y, max, level );

        data += point_size;
        x += x_offset;
        m.width += x_offset;
        last_point = code_point;
    }

    block->input( "vertex_scale", 2, point, true );
    block->input( "vertex_translate", 2, point, true );
    block->input( "coord_scale", 2, point, true );
    block->input( "coord_translate", 2, point, true );
    return glyphs;
}

OOE_NAMESPACE_END( ( ooe ) )
