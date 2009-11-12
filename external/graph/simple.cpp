/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <vector>

#include "foundation/maths/camera.hpp"

#include "external/graph/scene.hpp"
#include "external/graph/simple.hpp"

namespace
{
	using namespace ooe;

	class simple
		: private noncopyable, public scene::graph
	{
	public:
		simple( ooe::video& );

		virtual void insert( const scene::node& );
		virtual void draw( const camera& ) const;

	private:
		typedef std::vector< const scene::node* > vector_type;

		ooe::video& video;
		vector_type vector;
		mutable bool blend;
	};

//--- simple -------------------------------------------------------------------
	simple::simple( ooe::video& video_ )
		: video( video_ ), vector(), blend( false )
	{
		video.enable( video::blend_mode, false );
		video.blend( video::source_alpha, video::source_alpha_invert );
	}

	void simple::insert( const scene::node& node )
	{
		vector.push_back( &node );
	}

	void simple::draw( const camera& camera ) const
	{
		video.matrix( video::projection );
		video.matrix( camera.matrix()[ 0 ], video::load );
		video.matrix( video::model_view );

		typedef vector_type::const_reverse_iterator iterator;

		for ( iterator i = vector.rbegin(), end = vector.rend(); i != end; ++i )
		{
			const scene::node& node = **i;

		//	if ( !camera.includes( column( node.matrix[ 3 ] ) ) )
		//		continue;

			video.matrix( node.matrix[ 0 ], video::load );

			if ( node.blend != blend )
			{
				blend = !blend;
				video.enable( video::blend_mode, blend );
			}

			node.draw( video );
		}
	}
}

namespace ooe
{
	scene::graph* simple_create( video& video, u16, u16 )
	{
		return new simple( video );
	}
}
