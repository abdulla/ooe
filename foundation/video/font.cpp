/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <ft2build.h>
#include FT_FREETYPE_H

#include "foundation/utility/error.hpp"
#include "foundation/video/font.hpp"

namespace ooe
{
//--- font::library ------------------------------------------------------------
	font::library::library( void )
		: freetype()
	{
		if ( FT_Init_FreeType( &freetype ) )
			throw error::runtime( "font::library: " ) << "Library not initialised";
	}

	font::library::~library( void )
	{
		FT_Done_FreeType( freetype );
	}

//--- font::face ---------------------------------------------------------------
	font::face::face( const library& library, const descriptor& desc )
		: memory( desc ), face_( 0 )
	{
		if ( FT_New_Memory_Face( library.freetype, memory.as< u8 >(), memory.size(), 0, &face_ ) )
			throw error::runtime( "font: " ) << "Unable to open font face";
	}

	font::face::~face( void )
	{
		FT_Done_Face( face_ );
	}

	bool font::face::character( bitmap& requested, u32 index )
	{
		if ( FT_Load_Char( face_, index, FT_LOAD_RENDER | FT_LOAD_PEDANTIC ) )
			return false;

		FT_GlyphSlot glyph = face_->glyph;
		requested.width = glyph->bitmap.width;
		requested.height = glyph->bitmap.rows;
		requested.left = glyph->bitmap_left;
		requested.top = glyph->bitmap_top;
		requested.x = glyph->advance.x >> 6;
		requested.y = glyph->advance.y >> 6;
		requested.pointer = glyph->bitmap.buffer;
		return true;
	}

	bool font::face::size( u32 size_ )
	{
		return !FT_Set_Pixel_Sizes( face_, size_ - 1, 0 );
	}

	std::string font::face::name( u8 type )
	{
		return type == family ? face_->family_name : face_->style_name;
	}

	u32 font::face::number( u8 type )
	{
		return type == glyphs ? face_->num_glyphs : face_->num_fixed_sizes;
	}
}
