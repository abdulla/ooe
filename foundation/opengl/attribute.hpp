/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_ATTRIBUTE_HPP
#define OOE_FOUNDATION_OPENGL_ATTRIBUTE_HPP

#include "foundation/general/video.hpp"

namespace ooe
{
	namespace opengl
	{
		struct attribute;
	}

	struct opengl::attribute
		: private noncopyable, public ooe::attribute
	{
		s32 id;
		u8 size;

		attribute( u32, const c8*, type );
		virtual ~attribute( void );
	};
}

#endif	// OOE_FOUNDATION_OPENGL_ATTRIBUTE_HPP
