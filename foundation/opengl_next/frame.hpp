/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_NEXT_FRAME_HPP
#define OOE_FOUNDATION_OPENGL_NEXT_FRAME_HPP

#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

class default_frame
	: public ooe::frame
{
public:
	default_frame( u32, u32 );
	virtual ~default_frame( void );

	virtual void write( const frame_type& );
	virtual void read( buffer_type&, image::type );
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
	typedef std::vector< u32 > colour_vector;

	const u32 id;
	const u32 width;
	const u32 height;
	bool check;
	colour_vector colours;

	frame( u32, u32, u32 );
	virtual ~frame( void );

	virtual void write( const frame_type& );
	virtual void read( buffer_type&, image::type );
	virtual void output( attachment_type, const texture_type& );
	virtual void output( attachment_type, const target_type& );

private:
	const u32 program;
};

OOE_NAMESPACE_END( ( ooe )( opengl ) )

#endif	// OOE_FOUNDATION_OPENGL_NEXT_FRAME_HPP
