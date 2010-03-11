/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_ATTRIBUTE_HPP
#define OOE_FOUNDATION_OPENGL_ATTRIBUTE_HPP

#include "foundation/visual/video.hpp"

namespace ooe
{
	namespace opengl
	{
		struct attribute;
	}

	struct opengl::attribute
		: public ooe::attribute
	{
		s32 id;
		u8 size;

		attribute( u32, const c8*, type );
		virtual ~attribute( void );
	};
}

#endif	// OOE_FOUNDATION_OPENGL_ATTRIBUTE_HPP
