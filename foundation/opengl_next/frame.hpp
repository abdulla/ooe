/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_NEXT_FRAME_HPP
#define OOE_FOUNDATION_OPENGL_NEXT_FRAME_HPP

#include <set>

#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

struct frame
	: public ooe::frame
{
public:
	typedef tuple< s32, attachment_type, texture_type > texture_tuple;
	typedef std::vector< texture_tuple > texture_vector;
	typedef tuple< s32, attachment_type, target_type > target_tuple;
	typedef std::vector< target_tuple > target_vector;
	typedef std::set< std::string > name_set;

	const u32 id;
	texture_vector textures;
	target_vector targets;

	frame( u32 );
	virtual ~frame( void );

	virtual void output( const std::string&, attachment_type, const texture_type& );
	virtual void output( const std::string&, attachment_type, const target_type& );

private:
	const u32 program;
	name_set names;
};

OOE_NAMESPACE_END( ( ooe )( opengl ) )

#endif	// OOE_FOUNDATION_OPENGL_NEXT_FRAME_HPP
