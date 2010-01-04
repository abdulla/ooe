/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_EXTERNAL_SCENE_OCTREE_HPP
#define OOE_EXTERNAL_SCENE_OCTREE_HPP

#include <list>

namespace ooe
{
	typedef std::list< node* > octree_list;
	class octree_node;

	class octree
	{
	public:
		octree( void );
		~octree( void );

	private:
		octree_node* root;
		octree_list list;
	};

	class octree_node
	{
	public:
		octree_node( const aabb&, octree_list& );

		const ooe::aabb& aabb( void ) const;

	private:
		ooe::aabb box;
		octree_node* cube[ 2 ][ 2 ][ 2 ];

		octree_list& list;
		octree_list::iterator begin;
		octree_list::iterator end;
	};
}

#endif	// OOE_EXTERNAL_SCENE_OCTREE_HPP
