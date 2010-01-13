/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_SHADER_HPP
#define OOE_FOUNDATION_OPENGL_SHADER_HPP

#include "foundation/video/video.hpp"

namespace ooe
{
	namespace opengl
	{
		class shader;
		class program;
	}

	class opengl::shader
		: public ooe::shader
	{
	public:
		shader( type );
		virtual ~shader( void );

		virtual void compile( const c8*, u32 );

	private:
		u32 id;

		friend class program;
	};
}

#endif	// OOE_FOUNDATION_OPENGL_SHADER_HPP
