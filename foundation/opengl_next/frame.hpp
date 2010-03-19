/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_NEXT_FRAME_HPP
#define OOE_FOUNDATION_OPENGL_NEXT_FRAME_HPP

#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

struct frame
	: public ooe::frame
{
	const u32 id;

	frame( frame::type );
	virtual ~frame( void );

	virtual void draw( const frame_type& );
	virtual void draw( const batch& );
};

OOE_NAMESPACE_END( ( ooe )( opengl ) )

#endif	// OOE_FOUNDATION_OPENGL_NEXT_FRAME_HPP
