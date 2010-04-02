/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_FONT_SOURCE_HPP
#define OOE_COMPONENT_UI_FONT_SOURCE_HPP

#include "foundation/ui/virtual_texture.hpp"
#include "foundation/visual/font.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- font_source ----------------------------------------------------------------------------------
class font_source
	: public physical_source
{
public:
	font_source( const font::face&, u32 );

	virtual size_tuple size( void ) const;
	virtual image read( u32, u32, u16 );

private:
	font::face face;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_COMPONENT_UI_FONT_SOURCE_HPP
