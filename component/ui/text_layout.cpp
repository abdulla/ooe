/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cmath>

#include <utf8.h>

#include "component/ui/text_layout.hpp"
#include "foundation/utility/arithmetic.hpp"
#include "foundation/utility/binary.hpp"
#include "foundation/utility/miscellany.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

typedef text_vector::const_iterator text_iterator;
typedef std::string::const_iterator string_iterator;
const up_t point_size = 8 * sizeof( f32 ) + 4 * sizeof( u8 );
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
    u32 last_point;

    state( f32 x_, f32 y_, u32 font_size_, u32 last_point_ )
        : x( x_ ), y( y_ ), font_size( font_size_ ), last_point( last_point_ )
    {
    }
};

struct marker
{
    text_iterator i;
    string_iterator j;
    void* data;
    f32 width;

    marker( text_iterator i_, string_iterator j_, void* data_, f32 width_ )
        : i( i_ ), j( j_ ), data( data_ ), width( width_ )
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
        state.x = bit_shift( text.x, zoom );
        state.y += state.font_size;
        state.font_size = 1 << ( text.level + zoom );
        state.y += state.font_size * line_spacing;
        return true;

    default:
        return std::iswspace( code_point );
    }
}

void add_glyph( const font_source::glyph_type& glyph, const text& text, const state& state,
    void* data, u32 size, s8 shift, u8 level )
{
    const font::metric& metric = glyph._0;

    f32* point = add< f32 >( data, 0 );
    point[ 0 ] = bit_shift( metric.width, shift );
    point[ 1 ] = bit_shift( metric.height, shift );
    point[ 2 ] = std::floor( state.x + bit_shift( metric.left, shift ) );
    point[ 3 ] = std::ceil( state.y + state.font_size - bit_shift( metric.top, shift ) );

    point[ 4 ] = divide( metric.width << level, size );
    point[ 5 ] = divide( metric.height << level, size );
    point[ 6 ] = divide( glyph._1, size );
    point[ 7 ] = divide( glyph._2, size );

    u8* colour = add< u8 >( data, 8 * sizeof( f32 ) );
    colour[ 0 ] = text.red;
    colour[ 1 ] = text.green;
    colour[ 2 ] = text.blue;
    colour[ 3 ] = 255;
}

marker add_text( const font_source& source, virtual_texture& texture, const limit& limit,
    state& state, const marker& next, marker& line, marker& word, const text_iterator& i_end )
{
    const text& text = *next.i;
    u32 line_size = 1 << ( text.level + limit.zoom );

    if ( line_size > state.font_size )
    {
        state.x = bit_shift( line.i->x, limit.zoom );
        state.font_size = line_size;
        return line;
    }

    state.y = std::max< f32 >( state.y, bit_shift( text.y, limit.zoom ) );
    f32 width = limit.width - bit_shift( text.x, limit.zoom );
    void* data = next.data;
    s8 shift = inverse_clamp< s8 >( text.level + limit.zoom, limit.min, limit.max );
    u8 level = limit.max - clamp< s8 >( text.level + limit.zoom, limit.min, limit.max );

    for ( string_iterator j = next.j, j_end = text.data.end(); j != j_end; )
    {
        u32 code_point = utf8::next( j, j_end );

        // word boundary
        if ( handle_space( code_point, text, state, limit.zoom ) )
        {
            word = marker( next.i, j, data, 0 );
            continue;
        }
        else if ( state.last_point )
            state.x += source.kerning( state.last_point, code_point, level ) * exp2f( shift );

        font_source::glyph_type glyph = source.glyph( code_point, level );
        f32 advance = glyph._0.advance * exp2f( shift );

        // line boundary
        if ( state.x + advance > width )
        {
            handle_space( '\n', text, state, limit.zoom );

            if ( word.width + advance < width )
                return line = word;

            // if the word is larger than the line width, break up the word
            word.width = 0;
            state.last_point = 0;
            utf8::prior( j, text.data.begin() );
            line = marker( next.i, j, data, 0 );
            continue;
        }

        texture.load( glyph._0.width, glyph._0.height, glyph._1, glyph._2, level );
        add_glyph( glyph, text, state, data, limit.size, shift, level );

        data = add< void >( data, point_size );
        state.x += advance;
        word.width += advance;
        state.last_point = code_point;
    }

    text_iterator i = next.i + 1;
    return marker( i, i != i_end ? i->data.begin() : string_iterator(), data, 0 );
}

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- text -----------------------------------------------------------------------------------------
text::text( void )
    : data(), x( 0 ), y( 0 ), level( 4 ), red( 0 ), green( 0 ), blue( 0 )
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
    map_ptr map = point->map( buffer::write );

    limit limit( source, width, zoom );
    state state( bit_shift( text[ 0 ].x, zoom ), 0, 1 << ( text[ 0 ].level + zoom ), 0 );
    marker next( text.begin(), text[ 0 ].data.begin(), map->data, 0 );
    marker line = next;
    marker word = next;

    for ( text_iterator i = next.i, i_end = text.end(); i != i_end; i = next.i )
        next = add_text( source, texture, limit, state, next, line, word, i_end );

    block->input( "vertex_scale", block::f32_2, point );
    block->input( "vertex_translate", block::f32_2, point );
    block->input( "coord_scale", block::f32_2, point );
    block->input( "coord_translate", block::f32_2, point );
    block->input( "colour", block::u8_4, point );
    texture.input( block, "texture" );
    return glyphs;
}

OOE_NAMESPACE_END( ( ooe ) )
