/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_NEXT_GRAPHICS_HPP
#define OOE_FOUNDATION_OPENGL_NEXT_GRAPHICS_HPP

#include "foundation/opengl_next/context.hpp"
#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- graphics -------------------------------------------------------------------------------------
class OOE_VISIBLE graphics
	: private noncopyable
{
public:
	graphics( const ooe::view_data& );

	void draw( batch& );
	void swap( void );

private:
	const view_data& view;
	platform::context_type context;
};

OOE_NAMESPACE_END( ( ooe )( opengl ) )

#endif	// OOE_FOUNDATION_OPENGL_NG_GRAPHICS_HPP
