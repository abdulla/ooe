/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_LCD_FILTER_H

#include "foundation/utility/error.hpp"
#include "foundation/visual/font.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe )( font ) )

const u32 metric_flags = FT_LOAD_PEDANTIC | FT_LOAD_NO_SCALE;
const u32 bitmap_flags = FT_LOAD_PEDANTIC | FT_LOAD_RENDER | FT_LOAD_TARGET_LCD |
    FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT;

FT_Pos translate( face::bitmap_type subpixel )
{
    switch ( subpixel )
    {
    case face::red:
        return 0;

    case face::green:
        return 21;

    case face::blue:
        return 42;

    default:
        throw error::runtime( "font::face: " ) << "Unknown sub-pixel position: " << subpixel;
    }
}

OOE_ANONYMOUS_END( ( ooe )( font ) )

OOE_NAMESPACE_BEGIN( ( ooe )( font ) )

//--- library --------------------------------------------------------------------------------------
library::library( void )
    : freetype()
{
    if ( FT_Init_FreeType( &freetype ) )
        throw error::runtime( "font::library: " ) << "Unable to initialise library";
    else if ( FT_Library_SetLcdFilter( freetype, FT_LCD_FILTER_LIGHT ) )
        throw error::runtime( "font::library: " ) << "Unable to set LCD filter";
}

library::~library( void )
{
    if ( FT_Done_FreeType( freetype ) )
        OOE_CONSOLE( "font::library: " << "Unable to destroy library" );
}

//--- metric ---------------------------------------------------------------------------------------
metric::metric( f32 left_, f32 top_, f32 advance_ )
    : left( left_ ), top( top_ ), advance( advance_ )
{
}

//--- bitmap ---------------------------------------------------------------------------------------
bitmap::bitmap( u16 width_, u16 height_, s32 pitch_, const u8* data_ )
    : width( width_ ), height( height_ ), pitch( pitch_ ), data( data_ )
{
}

//--- face -----------------------------------------------------------------------------------------
face::face( const library& library, const descriptor& desc )
    : memory( desc ), face_()
{
    if ( FT_New_Memory_Face( library.freetype, memory.as< u8 >(), memory.size(), 0, &face_ ) )
        throw error::runtime( "font::face: " ) << "Unable to open font face";
    else if ( FT_Select_Charmap( face_, FT_ENCODING_UNICODE ) )
        throw error::runtime( "font::face: " ) << "Unable to select unicode character map";
}

face::~face( void )
{
    if ( FT_Done_Face( face_ ) )
        OOE_CONSOLE( "font::face: " << "Unable to close font face" );
}

std::string face::string( string_type type ) const
{
    switch ( type )
    {
    case family:
        return face_->family_name;

    case style:
        return face_->style_name;

    default:
        throw error::runtime( "font::face: " ) << "Unknown string type: " << type;
    }
}

u32 face::number( number_type type ) const
{
    switch ( type )
    {
    case glyphs:
        return face_->num_glyphs;

    case strikes:
        return face_->num_fixed_sizes;

    default:
        throw error::runtime( "font::face: " ) << "Unknown number type: " << type;
    }
}

u32 face::glyph_index( u32 code_point ) const
{
    u32 index = FT_Get_Char_Index( face_, code_point );

    if ( !index )
        throw error::runtime( "font::face: " ) <<
            "Unable to get glyph index of code point " << code_point;

    return index;
}

f32 face::kerning( u32 left, u32 right ) const
{
    FT_Vector delta;

    if ( FT_Get_Kerning( face_, left, right, FT_KERNING_UNSCALED, &delta ) )
        throw error::runtime( "font::face: " ) <<
            "Unable to get kerning for " << left << " and " << right;

    return delta.x / face_->units_per_EM;
}

metric face::metric( u32 index ) const
{
    if ( FT_Load_Glyph( face_, index, metric_flags ) )
        throw error::runtime( "font::face: " ) << "Unable to load glyph index " << index;

    FT_Glyph_Metrics& m = face_->glyph->metrics;
    f32 em = face_->units_per_EM;
    return font::metric( m.horiBearingX / em, m.horiBearingY / em, m.horiAdvance / em );
}

bitmap face::bitmap( u32 index, u32 size, bitmap_type subpixel ) const
{
    FT_Vector delta = { translate( subpixel ), 0 };
    FT_Set_Transform( face_, 0, &delta );

    if ( FT_Set_Pixel_Sizes( face_, size, 0 ) )
        throw error::runtime( "font::face: " ) << "Unable to set pixel size to " << size;
    else if ( FT_Load_Glyph( face_, index, bitmap_flags ) )
        throw error::runtime( "font::face: " ) << "Unable to load glyph index " << index;

    FT_Bitmap& b = face_->glyph->bitmap;
    return font::bitmap( b.width > 8 ? ( b.width / 3 ) - 2 : 0, b.rows, b.pitch, b.buffer );
}

OOE_NAMESPACE_END( ( ooe )( font ) )
