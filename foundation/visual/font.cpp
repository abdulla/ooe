/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "foundation/utility/error.hpp"
#include "foundation/visual/font.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( font ) )

//--- library --------------------------------------------------------------------------------------
library::library( void )
	: freetype()
{
	if ( FT_Init_FreeType( &freetype ) )
		throw error::runtime( "font::library: " ) << "Unable to initialise library";
}

library::~library( void )
{
	if ( FT_Done_FreeType( freetype ) )
		OOE_WARNING( "font::library", "Unable to destroy library" );
}

//--- metric ---------------------------------------------------------------------------------------
metric::metric( s32 left_, s32 top_, u32 x_, u32 y_, u32 width_, u32 height_ )
	: left( left_ ), top( top_ ), x( x_ ), y( y_ ), width( width_ ), height( height_ )
{
}

//--- bitmap ---------------------------------------------------------------------------------------
bitmap::bitmap( const font::metric& metric_, const u8* data_ )
	: metric( metric_ ), data( data_ )
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
		OOE_WARNING( "font::face", "Unable to close font face" );
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

bitmap face::character( up_t char_code, u32 size )
{
	if ( FT_Set_Pixel_Sizes( face_, size, 0 ) )
		throw error::runtime( "font::face: " ) << "Unable to set pixel size to " << size;
	else if ( FT_Load_Char( face_, char_code, FT_LOAD_RENDER | FT_LOAD_PEDANTIC ) )
		throw error::runtime( "font::face: " ) << "Unable to load character " << hex( char_code );

	FT_GlyphSlot glyph = face_->glyph;
	font::metric metric( glyph->bitmap_left, glyph->bitmap_top, glyph->advance.x >> 6,
		glyph->advance.y >> 6, glyph->bitmap.width, glyph->bitmap.rows );
	return bitmap( metric, glyph->bitmap.buffer );
}

OOE_NAMESPACE_END( ( ooe )( font ) )
