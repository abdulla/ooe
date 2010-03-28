/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_VIRTUAL_TEXTURE_HPP
#define OOE_COMPONENT_UI_VIRTUAL_TEXTURE_HPP

#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

class virtual_texture
{
public:
	virtual_texture( void );

private:
	texture_type page_table;
	texture_type page_cache;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_COMPONENT_UI_VIRTUAL_TEXTURE_HPP
