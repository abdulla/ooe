/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_LIGHT_HPP
#define OOE_FOUNDATION_OPENGL_LIGHT_HPP

#include "foundation/visual/video.hpp"

namespace ooe
{
	namespace opengl
	{
		class light;
	}

	class opengl::light
		: public ooe::light
	{
	public:
		light( u32, bool& );
		~light( void );

		virtual void parameter( const f32*, parameter_type );

	private:
		const u32 id;
		bool& used;
	};
}

#endif	// OOE_FOUNDATION_OPENGL_LIGHT_HPP
