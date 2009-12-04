/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_BUFFER_HPP
#define OOE_FOUNDATION_OPENGL_BUFFER_HPP

#include "foundation/general/video.hpp"

namespace ooe
{
	namespace opengl
	{
		class buffer;
	}

	class opengl::buffer
		: public ooe::buffer
	{
	public:
		buffer( type );
		virtual ~buffer( void );

		virtual void bind( void );
		virtual void unbind( void );
		virtual void* map( map_type );
		virtual void unmap( void );
		virtual void load( const void*, up_t, load_type );

	private:
		u32 target;
		u32 id;
	};
}

#endif	// OOE_FOUNDATION_OPENGL_BUFFER_HPP
