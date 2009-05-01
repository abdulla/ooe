/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_SCENE_TILE_HPP
#define OOE_FOUNDATION_SCENE_TILE_HPP

#include "foundation/io/memory.hpp"
#include "foundation/scene/cache.hpp"
#include "foundation/scene/graph.hpp"

namespace ooe
{
	struct tile_header;

	class OOE_VISIBLE tile
		: public node
	{
	public:
		tile( cache& );
		virtual ~tile( void ) {}

		virtual ooe::aabb aabb( void ) const;
		virtual void includes( const camera_instance&, render_queue& ) const;
		virtual range_type iterator_range( void ) const;

	private:
		ooe::program& program;
		texture_type height;
		video_data data;

		virtual bool insert( node&, graph::list_type& );
	};
}

#endif	// OOE_FOUNDATION_SCENE_TILE_HPP
