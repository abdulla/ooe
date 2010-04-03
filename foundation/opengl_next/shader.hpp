/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_NEXT_SHADER_HPP
#define OOE_FOUNDATION_OPENGL_NEXT_SHADER_HPP

#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

struct shader
	: public ooe::shader
{
	const u32 id;

	shader( const std::string&, const descriptor&, type );
	virtual ~shader( void );
};

OOE_NAMESPACE_END( ( ooe )( opengl ) )

#endif	// OOE_FOUNDATION_OPENGL_NEXT_SHADER_HPP
