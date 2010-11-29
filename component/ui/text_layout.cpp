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

struct limit
{
    u32 size;
    u8 max;
    u8 min;

    limit( const font_source& source )
        : size( source.size() ), max( log2f( source.font_size() ) ),
        min( max - log2f( size / source.page_size() ) )
    {
    }
};

u32 glyph_size( const text_vector& text )
{
    u32 glyphs = 0;

    for ( text_vector::const_iterator i = text.begin(), i_end = text.end(); i != i_end; ++i )
    {
        for ( iterator_type j = i->data.begin(), j_end = i->data.end(); j != j_end; )
        {
            if ( !std::iswspace( utf8::next( j, j_end ) ) )
                ++glyphs;
        }
    }

    return glyphs;
}

bool handle_space( u32 code_point, u32 start_x, u32 font_size, f32& x, f32& y )
{
    switch ( code_point )
    {
    case ' ':
        x += font_size >> 2;
        return true;

    case '\n':
        x = start_x;
        y += font_size * line_spacing;
        return true;

    default:
        return std::iswspace( code_point );
    }
}

void add_glyph( const font_source::glyph_type& glyph, f32* data, f32 x, f32 y, u32 size,
    u32 font_size, s8 shift, u8 level )
{
    const font::metric& metric = glyph._0;

    data[ 0 ] = bit_shift( metric.width, shift );
    data[ 1 ] = bit_shift( metric.height, shift );
    data[ 2 ] = round( x + bit_shift( metric.left, shift ) );
    data[ 3 ] = round( y + font_size - bit_shift( metric.top, shift ) );

    data[ 4 ] = divide( metric.width << level, size );
    data[ 5 ] = divide( metric.height << level, size );
    data[ 6 ] = divide( glyph._1, size );
    data[ 7 ] = divide( glyph._2, size );
}

void add_text( const font_source& source, virtual_texture& texture, const limit& limit,
    const text& text, f32*& data, f32& x, f32& y, f32 width )
{
    u32 font_size = 1 << text.level;
    s8 shift = inverse_clamp< s8 >( text.level, limit.min, limit.max );
    u8 level = limit.max - clamp( text.level, limit.min, limit.max );
    u32 last_point = 0;
    marker m( text.data.begin(), data, 0 );

    for ( iterator_type i = m.i, end = text.data.end(); i != end; )
    {
        u32 code_point = utf8::next( i, end );

        if ( handle_space( code_point, text.x, font_size, x, y ) )
        {
            m = marker( i, data, 0 );
            continue;
        }
        else if ( last_point )
            x += source.kerning( last_point, code_point, level ) * exp2f( shift );

        font_source::glyph_type glyph = source.glyph( code_point, level );
        f32 advance = glyph._0.advance * exp2f( shift );

        if ( x + advance > width )
        {
            x = text.x;
            y += font_size * line_spacing;

            // if the word is larger than the line width, break up the word
            if ( m.width + advance > width )
                utf8::prior( i, text.data.begin() );
            else
            {
                i = m.i;
                data = m.data;
            }

            m.width = 0;
            last_point = 0;
            continue;
        }

        texture.load( glyph._0.width, glyph._0.height, glyph._1, glyph._2, level );
        add_glyph( glyph, data, x, y, limit.size, font_size, shift, level );

        data += point_size;
        x += advance;
        m.width += advance;
        last_point = code_point;
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
text_layout::text_layout( const device_ptr& device_, page_cache& cache, font_source& source_ )
    : device( device_ ), source( source_ ), texture( device_, cache, source_ )
{
}

u32 text_layout::input( block_ptr& block, const text_vector& text, f32 width )
{
    u32 glyphs = glyph_size( text );

    if ( !glyphs )
        return 0;

    limit limit( source );
    buffer_ptr point = device->buffer( sizeof( f32 ) * point_size * glyphs, buffer::point );
    map_ptr map = point->map( buffer::write );
    f32* data = static_cast< f32* >( map->data );
    f32 x = text.front().x;
    f32 y = text.front().y;

    for ( text_vector::const_iterator i = text.begin(), end = text.end(); i != end; ++i )
        add_text( source, texture, limit, *i, data, x, y, width );

    block->input( "vertex_scale", block::f32_2, point, true );
    block->input( "vertex_translate", block::f32_2, point, true );
    block->input( "coord_scale", block::f32_2, point, true );
    block->input( "coord_translate", block::f32_2, point, true );
    block->input( "colour", 0, 0, 0, 255 );
    texture.input( block, "texture" );
    return glyphs;
}

OOE_NAMESPACE_END( ( ooe ) )
