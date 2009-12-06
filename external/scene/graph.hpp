/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_EXTERNAL_SCENE_GRAPH_HPP
#define OOE_EXTERNAL_SCENE_GRAPH_HPP

#include <list>

#include "external/scene/effect.hpp"
#include "foundation/maths/camera.hpp"

namespace ooe
{
	class node;

//--- camera_instance ----------------------------------------------------------
	struct OOE_VISIBLE camera_instance
	{
		const ooe::camera& camera;	// current camera
		const vec3 position;		// current position
		const mat4 matrix;			// current matrix
		const ooe::frustum frustum;	// current frustum

		camera_instance( const ooe::camera& );
	};

//--- render_queue -------------------------------------------------------------
	class OOE_VISIBLE render_queue
	{
	public:
		typedef
			tuple< const mat4, const video_data&, const variable*, program*, texture*, texture* >
			insert_type;								// proper support for multiple textures?

		void insert( const insert_type& );
		void clear( void );
		void draw( const camera_instance&, video& );	// constify if using set

	private:
		typedef std::list< insert_type > list_type;

		list_type list;									// change to set?
	};

//--- graph --------------------------------------------------------------------
	class OOE_VISIBLE graph
	{
	public:
		typedef std::list< node* > list_type;

		void insert( const node& );
		bool insert( const node&, const node& );
		void draw( const camera&, video& ) const;

	private:
		list_type list;
	};

//--- node ---------------------------------------------------------------------
	class OOE_VISIBLE node
		: private noncopyable
	{
	public:
		typedef graph::list_type::const_iterator iterator;
		typedef tuple< iterator, iterator > range_type;

		virtual ~node( void );

		virtual ooe::aabb aabb( void ) const = 0;
		virtual void includes( const camera_instance&, render_queue& ) const = 0;
		// return iterator range from graph::list that specifies child nodes
		virtual range_type iterator_range( void ) const = 0;

	protected:
		// called by graph when asked to insert node as child of another
		virtual bool insert( node&, graph::list_type& ) = 0;

		friend class graph;
	};

//--- utility ------------------------------------------------------------------
	video_data aabb_data( const video&, const aabb&, f32, f32, f32 ) OOE_VISIBLE;
}

#endif	// OOE_EXTERNAL_SCENE_GRAPH_HPP
