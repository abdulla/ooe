/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_TILE_SOURCE_HPP
#define OOE_COMPONENT_UI_TILE_SOURCE_HPP

#include "component/ui/virtual_texture.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

typedef std::map< std::string, decoder_type > decoder_map;

//--- tile_source ----------------------------------------------------------------------------------
class tile_source
	: public physical_source
{
public:
	tile_source( const std::string&, const decoder_map& ) OOE_VISIBLE;
	virtual ~tile_source( void );

	virtual u32 size( void ) const;
	virtual image::type format( void ) const;
	virtual u16 page_size( void ) const;

private:
	const std::string root;
	std::string type;
	decoder_type decoder;

	u32 size_;
	image::type format_;
	u16 page_size_;
	u8 level_limit;

	virtual image read( const pyramid_index& );
};

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_COMPONENT_UI_TILE_SOURCE_HPP