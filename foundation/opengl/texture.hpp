/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_TEXTURE_HPP
#define OOE_FOUNDATION_OPENGL_TEXTURE_HPP

#include "foundation/general/video.hpp"

namespace ooe
{
	namespace opengl
	{
		class texture;
	}

	class opengl::texture
		: public ooe::texture
	{
	public:
		texture( void );
		virtual ~texture( void );

		virtual void bind( void );
		virtual void unbind( void );
		virtual void environment( environment_type );
		virtual void parameter( parameter_type );
		virtual void load( const void*, u32, u32, u8, u8 );

	private:
		u32 id;
	};
}

#endif	// OOE_FOUNDATION_OPENGL_TEXTURE_GL_HPP
