/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <vector>

#include "foundation/utility/miscellany.hpp"

#include "external/graph/flat.hpp"

namespace
{
	using namespace ooe;

	class flat
		: private noncopyable, public scene::graph
	{
	public:
		flat( ooe::video&, u16, u16 );

		virtual void insert( const scene::node& );
		virtual void draw( const camera& ) const;

	private:
		typedef std::vector< const scene::node* > vector_type;

		ooe::video& video;
		vector_type vector;
		mat4 projection;
		mutable bool blend;
	};

//--- flat ---------------------------------------------------------------------
	flat::flat( ooe::video& video_, u16 width, u16 height )
		: video( video_ ), vector(), projection( orthographic( 0, width, 0, height ) ),
		blend( false )
	{
		video.enable( video::blend_mode, false );
		video.blend( video::source_alpha, video::source_alpha_invert );
	}

	void flat::insert( const scene::node& node )
	{
		vector.push_back( &node );
	}

	void flat::draw( const camera& ) const
	{
		video.matrix( video::projection );
		video.matrix( projection[ 0 ], video::load );
		video.matrix( video::model_view );

		for ( vector_type::const_iterator i = vector.begin(), end = vector.end(); i != end; ++i )
		{
			const scene::node& node = **i;
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
	scene::graph* flat_create( video& video, u16 width, u16 height )
	{
		return new flat( video, width, height );
	}
}
