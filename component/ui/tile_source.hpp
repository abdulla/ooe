/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_FONT_SOURCE_HPP
#define OOE_COMPONENT_UI_FONT_SOURCE_HPP

#include "component/ui/virtual_texture.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

typedef std::map< std::string, image ( * )( const descriptor& ) > decoder_map;

//--- tile_source ----------------------------------------------------------------------------------
class tile_source
	: public physical_source
{
public:
	tile_source( const std::string&, const decode_map& );
	virtual ~tile_source( void );

private:
	const std::string root;
	const decode_map& map;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_COMPONENT_UI_FONT_SOURCE_HPP
