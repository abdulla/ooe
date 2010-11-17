/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cmath>

#include <utf8.h>

#include "component/ui/text_layout.hpp"
#include "foundation/utility/arithmetic.hpp"
#include "foundation/utility/binary.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

typedef std::string::const_iterator iterator_type;
const up_t point_size = 8;
const f32 line_spacing = 1.5;

struct marker
{
    iterator_type i;
    f32* data;
    f32 width;

    marker( iterator_type i_, f32* data_, f32 width_ )
        : i( i_ ), data( data_ ), width( width_ )
    {
    }
};

void add_glyph( const font_source::glyph_type& glyph, f32* data, u32 size, f32 x, f32 y,
    u32 font_size, u8 level, s8 shift )
{
    const font::metric& metric = glyph._0;

    data[ 0 ] = bit_shift( metric.width, shift );
    data[ 1 ] = bit_shift( metric.height, shift );
    data[ 2 ] = std::floor( x + bit_shift( metric.left, shift ) );
    data[ 3 ] = std::ceil( y + font_size - bit_shift( metric.top, shift ) );

    data[ 4 ] = divide( metric.width << level, size );
    data[ 5 ] = divide( metric.height << level, size );
    data[ 6 ] = divide( glyph._1, size );
    data[ 7 ] = divide( glyph._2, size );
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

bool handle_space( u32 code_point, f32& x, f32& y, u32 font_size )
{
    switch ( code_point )
    {
    case ' ':
        x += font_size >> 2;
        return true;

    case '\n':
        x = 0;
        y += font_size * line_spacing;
        return true;

    default:
        return std::iswspace( code_point );
    }
}

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- text -----------------------------------------------------------------------------------------
text::text( void )
    : data(), x( 0 ), y( 0 ), level( 4 ), red( 0 ), green( 0 ), blue( 0 )
{
}

//--- text_layout ----------------------------------------------------------------------------------
text_layout::
    text_layout( const device_type& device_, page_cache& cache, font_source& source_ )
    : device( device_ ), source( source_ ), texture( device_, cache, source_ )
{
}

u32 text_layout::input( block_type& block, const text& text, f32 width )
{
    u32 glyphs = glyph_size( text.data.begin(), text.data.end() );
    u32 font_size = 1 << text.level;

    if ( !glyphs || text.x + font_size > width )
        return 0;

    u32 size = source.size();
    u8 level_limit = log2f( size / source.page_size() );
    u8 level_max = log2f( source.font_size() );
    u8 level_min = level_max - level_limit;
    u8 level = level_max - clamp( text.level, level_min, level_max );
    s8 shift = inverse_clamp< s8 >( text.level, level_min, level_max );
    width -= text.x;
    f32 x = 0;
    f32 y = text.y;
    u32 code_point = 0;
    u32 last_point;

    buffer_type point = device->buffer( sizeof( f32 ) * point_size * glyphs, buffer::point );
    map_type map = point->map( buffer::write );
    f32* data = static_cast< f32* >( map->data );
    marker m( text.data.begin(), data, 0 );

    for ( iterator_type i = m.i, end = text.data.end(); i != end; )
    {
        code_point = utf8::next( i, end );

        if ( handle_space( code_point, x, y, font_size ) )
        {
            m = marker( i, data, 0 );
            continue;
        }
        else if ( x > 0 )
            x += source.kerning( last_point, code_point, level ) * exp2f( shift );

        font_source::glyph_type glyph = source.glyph( code_point, level );
        f32 advance = glyph._0.advance * exp2f( shift );

        if ( x + advance > width )
        {
            x = 0;
            y += font_size * line_spacing;

            if ( m.width + advance > width )
                utf8::prior( i, text.data.begin() );
            else
            {
                i = m.i;
                data = m.data;
            }

            m.width = 0;
            continue;
        }

        texture.load( glyph._0.width, glyph._0.height, glyph._1, glyph._2, level );
        add_glyph( glyph, data, size, x + text.x, y, font_size, level, shift );

        data += point_size;
        x += advance;
        m.width += advance;
        last_point = code_point;
    }

    block->input( "vertex_scale", 2, point, true );
    block->input( "vertex_translate", 2, point, true );
    block->input( "coord_scale", 2, point, true );
    block->input( "coord_translate", 2, point, true );
    block->input( "colour", text.red, text.green, text.blue, 255 );
    texture.input( block, "texture" );
    return glyphs;
}

OOE_NAMESPACE_END( ( ooe ) )
