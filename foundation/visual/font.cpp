/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_LCD_FILTER_H

#include "foundation/utility/error.hpp"
#include "foundation/visual/font.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe )( font ) )

const u32 load_flags = FT_LOAD_RENDER | FT_LOAD_PEDANTIC | FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING |
    FT_LOAD_NO_AUTOHINT | FT_LOAD_TARGET_LCD;

u32 glyph_index( FT_Face face, u32 code_point )
{
    u32 index = FT_Get_Char_Index( face, code_point );

    if ( !index )
        throw error::runtime( "font::face: " ) << "Unable to get index of " << code_point;

    return index;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe )( font ) )

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
metric::metric( s32 left_, s32 top_, s32 advance_, u16 width_, u16 height_ )
    : left( left_ >> 6 ), top( top_ >> 6 ), advance( advance_ / 64. ),
    width( ( width_ / 3 ) - 2 ), height( height_ )
{
}

//--- bitmap ---------------------------------------------------------------------------------------
bitmap::bitmap( const font::metric& metric_, s32 pitch_, const u8* data_ )
    : metric( metric_ ), pitch( pitch_ ), data( data_ )
{
}

//--- face -----------------------------------------------------------------------------------------
face::face( const library& library, const descriptor& desc )
    : memory( desc ), face_()
{
    if ( FT_New_Memory_Face( library.freetype, memory.as< u8 >(), memory.size(), 0, &face_ ) )
        throw error::runtime( "font::face: " ) << "Unable to open font face";
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

    case first:
        return FT_Get_First_Char( face_, 0 );

    default:
        throw error::runtime( "font::face: " ) << "Unknown number type: " << type;
    }
}

f32 face::kerning( u32 left, u32 right, u32 size ) const
{
    if ( FT_Set_Pixel_Sizes( face_, size, 0 ) )
        throw error::runtime( "font::face: " ) << "Unable to set pixel size to " << size;

    u32 i = glyph_index( face_, left );
    u32 j = glyph_index( face_, right );
    FT_Vector delta;

    if ( FT_Get_Kerning( face_, i, j, FT_KERNING_UNFITTED, &delta ) )
        throw error::runtime( "font::face: " ) <<
            "Unable to get kerning for " << left << " and " << right;

    return delta.x / 64.;
}

bitmap face::bitmap( u32 code_point, u32 size ) const
{
    if ( FT_Set_Pixel_Sizes( face_, size, 0 ) )
        throw error::runtime( "font::face: " ) << "Unable to set pixel size to " << size;
    else if ( FT_Load_Char( face_, code_point, load_flags ) )
        throw error::runtime( "font::face: " ) << "Unable to load " << code_point;

    FT_Glyph_Metrics& m = face_->glyph->metrics;
    FT_Bitmap& b = face_->glyph->bitmap;
    font::metric metric( m.horiBearingX, m.horiBearingY, m.horiAdvance, b.width, b.rows );
    return font::bitmap( metric, b.pitch, b.buffer );
}

OOE_NAMESPACE_END( ( ooe )( font ) )
