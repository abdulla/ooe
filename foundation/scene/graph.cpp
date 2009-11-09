/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/scene/graph.hpp"
#include "foundation/utility/convert.hpp"
#include "foundation/utility/error.hpp"

namespace
{
	using namespace ooe;

	bool less_than( const render_queue::insert_type& a, const render_queue::insert_type& b )
	{
		return a._3 < b._3 || ( !( b._3 < a._3 ) && a._4 < b._4 ) ||
			( !( b._4 < a._4 ) && a._5 < b._5 );
	}

	void bind_if( program*& last, program* next )
	{
		if ( last == next )
			return;

		if ( next )
			next->bind();
		else if ( last )
			last->unbind();

		last = next;
	}

	void bind_if( texture*& last, texture* next, video& video, u8 unit )
	{
		if ( last == next )
			return;

		video.unit( unit );

		if ( next )
			next->bind();
		else if ( last )
			last->unbind();

		last = next;
	}
}

namespace ooe
{
//--- camera_instance ----------------------------------------------------------
	camera_instance::camera_instance( const ooe::camera& camera_ )
		: camera( camera_ ), position( camera.position() ), matrix( camera.matrix() ),
		frustum( matrix )
	{
	}

//--- render_queue -------------------------------------------------------------
	void render_queue::insert( const insert_type& value )
	{
		list.push_back( value );
	}

	void render_queue::clear( void )
	{
		list.clear();
	}

	void render_queue::draw( const camera_instance& instance, video& video )
	{
		video.matrix( video::projection );
		video.matrix( instance.matrix[ 0 ], video::load );
		video.matrix( video::model_view );

		list.sort( &less_than );
		// last not stored across draws due to possible non-existance
		program* last_program = 0;
		texture* last_texture_1 = 0;
		texture* last_texture_2 = 0;

		for ( list_type::const_iterator i = list.begin(), end = list.end(); i != end; ++i )
		{
			const insert_type& tuple = *i;
			bind_if( last_program, tuple._3 );
			bind_if( last_texture_1, tuple._4, video, 0 );
			bind_if( last_texture_2, tuple._5, video, 1 );

			if ( tuple._2 )
				tuple._2->apply();

			video.matrix( tuple._0[ 0 ], video::load );
			video.draw( tuple._1 );
		}

		// make sure everything is unbound for next draw
		bind_if( last_program, 0 );
		bind_if( last_texture_1, 0, video, 0 );
		bind_if( last_texture_2, 0, video, 1 );
	}

//--- node ---------------------------------------------------------------------
	node::~node( void )
	{
	}

//--- utility ------------------------------------------------------------------
	video_data aabb_data( const video& video, const aabb& aabb, f32 r, f32 g, f32 b )
	{
		const f32 points[] =
		{
			aabb.min.x, aabb.min.y, aabb.min.z, r, g, b,	// near bottom left
			aabb.max.x, aabb.min.y, aabb.min.z, r, g, b,	// near bottom right
			aabb.max.x, aabb.max.y, aabb.min.z, r, g, b,	// near top right
			aabb.min.x, aabb.max.y, aabb.min.z, r, g, b,	// near top left

			aabb.min.x, aabb.min.y, aabb.max.z, r, g, b,	// far bottom left
			aabb.max.x, aabb.min.y, aabb.max.z, r, g, b,	// far bottom right
			aabb.max.x, aabb.max.y, aabb.max.z, r, g, b,	// far top right
			aabb.min.x, aabb.max.y, aabb.max.z, r, g, b,	// far top left
		};

		const u16 indices[] =
		{
			0, 1, 1, 2, 2, 3, 3, 0,	// near square
			4, 5, 5, 6, 6, 7, 7, 4,	// far square
			0, 4, 1, 5, 2, 6, 3, 7	// connecting lines
		};

		buffer_ptr point_buffer = video.buffer( buffer::point );
		point_buffer->bind();
		point_buffer->load( points, sizeof( points ), buffer::static_draw );

		buffer_ptr index_buffer = video.buffer( buffer::index );
		index_buffer->bind();
		index_buffer->load( indices, sizeof( indices ), buffer::static_draw );

		const video_data data =
		{
			video_data::line_list,
			sizeof( indices ) / sizeof( u16 ),
			3,
			new video_data::layout_type[ 3 ],
			0,
			point_buffer,
			index_buffer
		};

		data.layout[ 0 ] = video_data::vertex_3;
		data.layout[ 1 ] = video_data::colour_3;
		data.layout[ 2 ] = video_data::index_16;

		return data;
	}
}
