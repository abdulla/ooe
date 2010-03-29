/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_VIRTUAL_TEXTURE_HPP
#define OOE_COMPONENT_UI_VIRTUAL_TEXTURE_HPP

#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- physical_source ------------------------------------------------------------------------------
struct physical_source
{
	typedef tuple< u32, u32 > size_tuple;

	virtual size_tuple size( void ) const = 0;
	virtual image read( u32, u32, u8 ) = 0;
};

//--- virtual_texture ------------------------------------------------------------------------------
class virtual_texture
{
public:
	virtual_texture( physical_source& );

private:
	physical_source& source;
	texture_type page_table;
	texture_type page_cache;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_COMPONENT_UI_VIRTUAL_TEXTURE_HPP
