/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_SCENE_CHUNKED_HPP
#define OOE_FOUNDATION_SCENE_CHUNKED_HPP

#include "foundation/io/memory.hpp"
#include "foundation/parallel/scheduler.hpp"
#include "foundation/scene/cache.hpp"
#include "foundation/scene/graph.hpp"
#include "foundation/utility/atom.hpp"

namespace ooe
{
	struct vertex_header;
	struct index_header;
	struct tqt_header;
	struct chunk_header;
	struct chunked_header;

	struct chunk_args;
	struct update_args;
	struct includes_args;

//--- tqt ----------------------------------------------------------------------
	class tqt
	{
	public:
		tqt( const descriptor& );

		uncompressed_image load( u32, u32, u32 ) const;
		u32 depth( void ) const;

	private:
		const ooe::memory memory;
		const tqt_header& header;
	};

//--- chunk --------------------------------------------------------------------
	class chunk
	{
	public:
		enum state_type
		{
			none,
			wait,	// doing parallel operation
			half,
			full
		};

		typedef shared_ptr< ooe::chunk > type;
		typedef scoped_array< type > array_type;

		chunk( chunk_args&, chunk*, u16 );

		void parallel_load( const tqt&, const attribute_type& );
		void parallel_unload( void );
		void serial_load( const video&, u32 );
		void serial_unload( void );

		void unload_subtree( scheduler& );
		void find_neighbours( const chunked_header&, const array_type& );
		void update( const update_args& );
		void includes( const includes_args&, geometry::intersection ) const;
		ooe::aabb aabb( void ) const;

	private:
		const chunk_header& header;
		const vertex_header& vh;
		const index_header& ih;

		chunk* parent;
		chunk* child[ 4 ];

		const ooe::aabb box;
		texture_type tex_1;
		texture_type* tex_2;
		video_data data;
		variable var;

		atom< u32 > state;
		shared_ptr< uncompressed_image > image;
		shared_array< f32 > point;

		u16 lod;
		f32 morph;
		bool split;
	};

//--- chunked ------------------------------------------------------------------
	class OOE_VISIBLE chunked
		: public node
	{
	public:
		chunked( const std::string&, ooe::scheduler&, cache&, radian, u32, f32 );
		virtual ~chunked( void ) {}

		virtual ooe::aabb aabb( void ) const;
		virtual void includes( const camera_instance&, render_queue& ) const;
		virtual range_type iterator_range( void ) const;

	private:
		const ooe::tqt tqt;
		const ooe::memory memory;
		const chunked_header& header;
		ooe::chunk::array_type chunk;
		f32 lod;

		ooe::scheduler& scheduler;
		const ooe::video& video;
		ooe::program& program;
		attribute_type delta;

		virtual bool insert( node&, graph::list_type& );
	};
}

#endif	// OOE_FOUNDATION_SCENE_CHUNKED_HPP
