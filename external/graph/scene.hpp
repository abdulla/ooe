/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_EXTERNAL_GRAPH_SCENE_HPP
#define OOE_EXTERNAL_GRAPH_SCENE_HPP

#include "foundation/general/camera.hpp"
#include "foundation/general/video.hpp"

namespace ooe
{
	namespace scene
	{
		struct node;
		struct graph;

		mat4 rotate( f32, f32, f32 ) OOE_VISIBLE;
		mat4 translate( f32, f32, f32 ) OOE_VISIBLE;
		mat4 scale( f32, f32, f32 ) OOE_VISIBLE;
	}

//--- scene::node --------------------------------------------------------------
	struct scene::node
	{
		bool blend;
		mat4 matrix;

		node( bool blend_ = false, const mat4& matrix_ = mat4() )
			: blend( blend_ ), matrix( matrix_ )
		{
		}

		virtual ~node( void ) {}

		virtual node* copy( void ) const = 0;
		virtual void draw( video& ) const = 0;
	};

//--- scene::graph -------------------------------------------------------------
	struct scene::graph
	{
		virtual ~graph( void ) {}

		virtual void insert( const node& ) = 0;
		virtual void draw( const camera& ) const = 0;
	};
}

#endif	// OOE_EXTERNAL_GRAPH_SCENE_HPP
