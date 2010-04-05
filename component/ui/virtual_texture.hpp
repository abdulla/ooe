/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_VIRTUAL_TEXTURE_HPP
#define OOE_COMPONENT_UI_VIRTUAL_TEXTURE_HPP

#include <list>

#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- physical_source ------------------------------------------------------------------------------
struct physical_source
{
	typedef tuple< u32, u32 > dimension_type;

	virtual ~physical_source( void ) {}

	virtual dimension_type dimension( void ) const = 0;
	virtual image::type format( void ) const = 0;
	virtual u16 page_size( void ) const = 0;

	virtual image read( u32, u32, u8 ) = 0;
};

//--- virtual_texture ------------------------------------------------------------------------------
class virtual_texture
{
public:
	virtual_texture( const device_type&, physical_source&, u32, u32 );

	program_type program( const device_type&, shader_vector& ) const;
	block_type block( const program_type&, const buffer_type& ) const;
	frame_type frame( const program_type& ) const;
	void write( const frame_type&, frame_type& );

private:
	typedef std::list< buffer_type > readback_type;
	typedef tuple< u32 /* x */, u32 /* y */, bool /* locked */ > cache_tuple;
	typedef std::list< cache_tuple > cache_type;

	physical_source& source;
	u32 frame_width;
	u32 frame_height;
	u32 cache_dimension;

	shader_type shader;
	texture_type page_table;
	texture_type page_cache;

	target_type fragment_target;
	target_type writeback_target;
	readback_type readback_list;
	// cache_type free_list;
	// cache_type used_list;	// lru-sorted
};

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_COMPONENT_UI_VIRTUAL_TEXTURE_HPP
