/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_CONTEXT_HPP
#define OOE_FOUNDATION_OPENGL_CONTEXT_HPP

#include "foundation/opengl/context_forward.hpp"
#include "foundation/utility/macro.hpp"

namespace ooe
{
	struct view_data;

	platform::context context_construct( const view_data& ) OOE_VISIBLE;
	void context_destruct( const view_data&, platform::context ) OOE_VISIBLE;
	bool context_current( const view_data&, platform::context ) OOE_VISIBLE;
	bool context_sync( const view_data&, platform::context, bool ) OOE_VISIBLE;
	void context_swap( const view_data&, platform::context ) OOE_VISIBLE;

	void setup_context( const view_data&, platform::context ) OOE_VISIBLE;
}

#endif	// OOE_FOUNDATION_OPENGL_CONTEXT_HPP
