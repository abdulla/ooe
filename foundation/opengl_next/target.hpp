/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_NEXT_TARGET_HPP
#define OOE_FOUNDATION_OPENGL_NEXT_TARGET_HPP

#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

struct target
	: public ooe::target
{
	const u32 id;

	target( u32, u32, u8 );
	virtual ~target( void );
};

OOE_NAMESPACE_END( ( ooe )( opengl ) )

#endif	// OOE_FOUNDATION_OPENGL_NEXT_TARGET_HPP
