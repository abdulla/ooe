/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_FONT_SOURCE_HPP
#define OOE_COMPONENT_UI_FONT_SOURCE_HPP

#include "component/ui/virtual_texture.hpp"
#include "foundation/visual/font.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- font_source ----------------------------------------------------------------------------------
class font_source
	: public physical_source
{
public:
	typedef tuple< u32, u32 > glyph_type;

	font_source( font::face&, u32, const std::string& ) OOE_VISIBLE;
	virtual ~font_source( void );

	virtual u32 size( void ) const;
	virtual image::type format( void ) const;
	virtual u16 page_size( void ) const;

	u32 font_size( void ) const;
	glyph_type glyph( up_t ) const;

	virtual image read( u32, u32, u8 );

private:
	font::face& face;
	const u32 face_size;
	const std::string root;

	const u32 source_size;
	const u32 glyphs;
	const u32 first;
	const u32 level_limit;

	ooe::mutex mutex;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_COMPONENT_UI_FONT_SOURCE_HPP
