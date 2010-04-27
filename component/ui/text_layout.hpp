/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_TEXT_LAYOUT_HPP
#define OOE_COMPONENT_UI_TEXT_LAYOUT_HPP

#include "component/ui/font_source.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- text_layout ----------------------------------------------------------------------------------
class text_layout
{
public:
	text_layout( const device_type&, virtual_texture&, const font_source& );

	block_type block( const program_type&, const std::string&, u8 );

private:
	const device_type& device;
	virtual_texture& vt;
	const font_source& source;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_COMPONENT_UI_TEXT_LAYOUT_HPP
