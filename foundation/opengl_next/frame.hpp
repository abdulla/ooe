/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_NEXT_FRAME_HPP
#define OOE_FOUNDATION_OPENGL_NEXT_FRAME_HPP

#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

class frame
	: public ooe::frame
{
public:
	typedef tuple< texture_type, attachment_type > texture_tuple;
	typedef std::vector< texture_tuple > texture_vector;
	typedef tuple< target_type, attachment_type > target_tuple;
	typedef std::vector< target_tuple > target_vector;

	const u32 id;
	texture_vector textures;
	target_vector targets;

	frame( void );
	virtual ~frame( void );

	virtual void write( const frame_type&, u8 );
	virtual void insert( const target_type&, attachment_type );
	virtual void insert( const texture_type&, attachment_type );

private:
	u32 width;
	u32 height;
};

OOE_NAMESPACE_END( ( ooe )( opengl ) )

#endif	// OOE_FOUNDATION_OPENGL_NEXT_FRAME_HPP
