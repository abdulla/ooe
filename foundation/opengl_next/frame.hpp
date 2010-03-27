/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_NEXT_FRAME_HPP
#define OOE_FOUNDATION_OPENGL_NEXT_FRAME_HPP

#include <map>

#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

class default_frame
	: public ooe::frame
{
public:
	default_frame( u32, u32 );
	virtual ~default_frame( void );

	virtual void write( const frame_type& );
	virtual void output( attachment_type, const texture_type& );
	virtual void output( attachment_type, const target_type& );

private:
	const u32 width;
	const u32 height;
};

struct frame
	: public ooe::frame
{
public:
	typedef std::map< s32, texture_type > texture_map;
	typedef std::map< s32, target_type > target_map;

	const u32 id;
	const u32 width;
	const u32 height;
	bool check;
	texture_map textures;
	target_map targets;

	frame( u32, u32, u32 );
	virtual ~frame( void );

	virtual void write( const frame_type& );
	virtual void output( attachment_type, const texture_type& );
	virtual void output( attachment_type, const target_type& );

private:
	const u32 program;
	u32 colour_index;
};

OOE_NAMESPACE_END( ( ooe )( opengl ) )

#endif	// OOE_FOUNDATION_OPENGL_NEXT_FRAME_HPP
