/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_EXTERNAL_GRAPH_MESH_HPP
#define OOE_EXTERNAL_GRAPH_MESH_HPP

#include "foundation/general/video.hpp"
#include "foundation/io/descriptor.hpp"

namespace ooe
{
	class OOE_VISIBLE mesh
	{
	public:
		mesh( const descriptor&, const video& );

		const video_data& get( void ) const;

	private:
		video_data data;
	};
}

#endif	// OOE_EXTERNAL_GRAPH_MESH_HPP
