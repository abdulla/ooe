/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_EXTERNAL_GENERAL_NODE_HPP
#define OOE_EXTERNAL_GENERAL_NODE_HPP

#include "external/graph/scene.hpp"

namespace ooe
{
	namespace scene
	{
		class data;

		class text;
		class mesh;
		class sight;
	}

	class OOE_VISIBLE scene::data
		: public node
	{
	public:
		data( bool, const video_data& );

		virtual data* copy( void ) const;
		virtual void draw( video& ) const;

	private:
		const video_data vdata;
	};

	class OOE_VISIBLE scene::text
		: public node
	{
	public:
		text( const texture_type&, const video_data& );

		virtual text* copy( void ) const;
		virtual void draw( video& ) const;

	private:
		const texture_type texture;
		const video_data data;
	};

	class OOE_VISIBLE scene::mesh
		: public node
	{
	public:
		mesh( ooe::texture&, const video_data& );

		virtual mesh* copy( void ) const;
		virtual void draw( video& ) const;

	private:
		ooe::texture& texture;
		const video_data& data;
	};

	class OOE_VISIBLE scene::sight
		: public node
	{
	public:
		sight( const video&, const video_data&, u16, u16 );

		virtual sight* copy( void ) const;
		virtual void draw( video& ) const;

		void update( const u8* );

	private:
		buffer_type buffer;
		texture_type texture;
		const video_data& data;
		u16 width;
		u16 height;
	};
}

#endif	// OOE_EXTERNAL_GENERAL_NODE_HPP
