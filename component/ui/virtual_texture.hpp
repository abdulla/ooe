/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_VIRTUAL_TEXTURE_HPP
#define OOE_COMPONENT_UI_VIRTUAL_TEXTURE_HPP

#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- physical_source ------------------------------------------------------------------------------
struct physical_source
{
	typedef tuple< u32, u32 > size_tuple;

	virtual ~physical_source( void ) {}
	virtual size_tuple size( void ) const = 0;
	virtual image::type format( void ) const = 0;
	virtual image read( u32, u32, u16 ) = 0;
};

//--- virtual_texture ------------------------------------------------------------------------------
class virtual_texture
{
public:
	virtual_texture( const device_type&, physical_source&, u32, u32 );

private:
	typedef std::vector< buffer_type > readback_vector;
	typedef tuple< u32, u32 > cache_tuple;
//	typedef std::list< cache_tuple > cache_list;

	physical_source& source;
	u32 pages_per_side;
	u32 position;

	texture_type page_table;
	texture_type page_cache;
	readback_vector readback;
	// cache_list free_list;
	// cache_list used_list;	// lru-sorted
};

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_COMPONENT_UI_VIRTUAL_TEXTURE_HPP
