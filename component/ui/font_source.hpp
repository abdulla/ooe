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
	font_source( font::face&, u32 ) OOE_VISIBLE;
	virtual ~font_source( void );

	virtual image::type format( void ) const;
	virtual u32 size( void ) const;
	virtual u16 page_size( void ) const;

	virtual image read( u32, u32, u8 );

private:
	font::face& face;
	const u32 size_;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_COMPONENT_UI_FONT_SOURCE_HPP
