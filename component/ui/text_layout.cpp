/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <cmath>

#include <utf8.h>

#include "component/ui/text_layout.hpp"
#include "foundation/utility/arithmetic.hpp"
#include "foundation/utility/binary.hpp"
#include "foundation/utility/miscellany.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

typedef text_vector::const_iterator text_iterator;
typedef std::string::const_iterator string_iterator;
const u8 point_size = 8 * sizeof( f32 ) + 4 * sizeof( u8 );
const f32 line_spacing = .5;
const f32 word_spacing = .25;

struct limit
{
    u32 size;
    f32 width;
    s8 zoom;
    u8 max;
    u8 min;

    limit( const font_source& source, f32 width_, s8 zoom_ )
        : size( source.size() ), width( width_ ), zoom( zoom_ ), max( log2f( source.font_size() ) ),
        min( max - log2f( size / source.page_size() ) )
    {
    }
};

struct state
{
    f32 x;
    f32 y;
    u32 font_size;
    u32 line_height;
    u32 last_index;

    state( f32 x_, f32 y_, u32 font_size_, u32 line_height_, u32 last_index_ )
        : x( x_ ), y( y_ ), font_size( font_size_ ), line_height( line_height_ ),
        last_index( last_index_ )
    {
    }
};

struct marker
{
    text_iterator i;
    string_iterator j;
    checked_map map;
    f32 width;

    marker( text_iterator i_, string_iterator j_, const checked_map& map_, f32 width_ )
        : i( i_ ), j( j_ ), map( map_ ), width( width_ )
    {
    }
};

u32 glyph_size( const text_vector& text )
{
    u32 glyphs = 0;

    for ( text_iterator i = text.begin(), i_end = text.end(); i != i_end; ++i )
    {
        for ( string_iterator j = i->data.begin(), j_end = i->data.end(); j != j_end; )
        {
            if ( !std::iswspace( utf8::next( j, j_end ) ) )
                ++glyphs;
        }
    }

    return glyphs;
}

bool handle_space( u32 code_point, const text& text, state& state, s8 zoom )
{
    switch ( code_point )
    {
    case ' ':
        state.x += ( 1 << ( text.level + zoom ) ) * word_spacing;
        return true;

    case '\n':
        state.x = bit_slide( text.x, zoom );
        state.y += state.line_height;
        state.line_height = 1 << ( text.level + zoom );
        state.y += state.line_height * line_spacing;
        return true;

    default:
        return std::iswspace( code_point );
    }
}

void add_glyph( const font_source::glyph_type& glyph, const colour& colour, const state& state,
    checked_map& map, u32 size, s8 slide, u8 level )
{
    const font::metric& metric = glyph._0;
    f32 coords[] =
    {
        bit_slide( glyph._1, slide ),
        bit_slide( glyph._2, slide ),
        state.x + metric.left * state.font_size,
        state.y + state.line_height - metric.top * state.font_size,

        divide( glyph._1 << level, size ),
        divide( glyph._2 << level, size ),
        divide( glyph._3, size ),
        divide( glyph._4, size )
    };

    map.write( coords, sizeof( coords ) );
    map.write( &colour, sizeof( colour ) );
}

marker add_text( const font_source& source, virtual_texture& texture, const limit& limit,
    state& state, const marker& next, marker& line, marker& word, const text_iterator& i_end )
{
    const text& text = *next.i;
    u32 font_size = 1 << ( text.level + limit.zoom );

    // if the font size is greater than the line height, adjust the line height
    if ( font_size > state.line_height )
    {
        // state.x = bit_slide( line.i->x, limit.zoom );
        state.line_height = font_size;
        return line;
    }

    state.font_size = font_size;
    state.y = std::max< f32 >( state.y, bit_slide( text.y, limit.zoom ) );
    f32 width = limit.width - bit_slide( text.x, limit.zoom );
    s8 slide = inverse_clamp< s8 >( text.level + limit.zoom, limit.min, limit.max );
    u8 level = limit.max - clamp< s8 >( text.level + limit.zoom, limit.min, limit.max );
    checked_map map = next.map;

    for ( string_iterator j = next.j, j_end = text.data.end(); j != j_end; )
    {
        string_iterator prior = j;
        u32 code_point = utf8::next( j, j_end );

        // word boundary
        if ( handle_space( code_point, text, state, limit.zoom ) )
        {
            word = marker( next.i, j, map, 0 );
            continue;
        }

        u32 glyph_index = source.glyph_index( code_point );
        font_source::glyph_type glyph = source.glyph( glyph_index, level );
        f32 advance = glyph._0.advance * state.font_size;

        if ( state.last_index )
            state.x += source.kerning( state.last_index, glyph_index ) * state.font_size;

        // line boundary (excluding glyphs wider than a line)
        if ( state.x + advance > width && prior != line.j )
        {
            handle_space( '\n', text, state, limit.zoom );

            if ( word.width + advance < width )
                return line = word;

            // if the word is larger than the line width, break up the word
            word.width = 0;
            state.last_index = 0;
            j = prior;
            line = word = marker( next.i, j, map, 0 );
            continue;
        }

        texture.load( glyph._1, glyph._2, glyph._3, glyph._4, level );
        add_glyph( glyph, text.colour, state, map, limit.size, slide, level );

        state.x += advance;
        word.width += advance;
        state.last_index = glyph_index;
    }

    text_iterator i = next.i + 1;
    string_iterator j = i != i_end ? i->data.begin() : string_iterator();
    return marker( i, j, map, 0 );
}

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- text -----------------------------------------------------------------------------------------
text::text( void )
    : data(), colour( 255, 255, 255, 255 ), x( 0 ), y( 0 ), level( 4 )
{
}

//--- text_layout ----------------------------------------------------------------------------------
text_layout::text_layout( const device_ptr& device_, page_cache& cache, font_source& source_ )
    : device( device_ ), source( source_ ), texture( device_, cache, source_ )
{
}

u32 text_layout::input( block_ptr& block, const text_vector& text, f32 width, s8 zoom )
{
    u32 glyphs = glyph_size( text );

    if ( !glyphs )
        return 0;

    buffer_ptr point = device->buffer( point_size * glyphs, buffer::point );
    checked_map map = point->map( buffer::write );
    u32 font_size = 1 << ( text[ 0 ].level + zoom );

    limit limit( source, width, zoom );
    state state( bit_slide( text[ 0 ].x, zoom ), 0, font_size, font_size, 0 );
    marker next( text.begin(), text[ 0 ].data.begin(), map, 0 );
    marker line = next;
    marker word = next;

    for ( text_iterator i = next.i, i_end = text.end(); i != i_end; i = next.i )
        next = add_text( source, texture, limit, state, next, line, word, i_end );

    block->input( "vertex_scale", block::f32_2, true, point );
    block->input( "vertex_translate", block::f32_2, true, point );
    block->input( "coord_scale", block::f32_2, true, point );
    block->input( "coord_translate", block::f32_2, true, point );
    block->input( "colour", block::u8_4, true, point );
    texture.input( block, "texture" );
    return glyphs;
}

OOE_NAMESPACE_END( ( ooe ) )
