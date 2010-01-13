/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_UNIFORM_HPP
#define OOE_FOUNDATION_OPENGL_UNIFORM_HPP

#include "foundation/video/video.hpp"

namespace ooe
{
	namespace opengl
	{
		class uniform;
	}

	class opengl::uniform
		: public ooe::uniform
	{
	public:
		typedef void ( * function_type )( s32, const void* );

		uniform( u32, const c8*, type );
		virtual ~uniform( void );

		virtual void load( const void* );

	private:
		s32 id;
		function_type function;
	};
}

#endif	// OOE_FOUNDATION_OPENGL_UNIFORM_HPP
