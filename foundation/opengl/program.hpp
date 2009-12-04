/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_PROGRAM_HPP
#define OOE_FOUNDATION_OPENGL_PROGRAM_HPP

#include "foundation/general/video.hpp"

namespace ooe
{
	namespace opengl
	{
		class program;
	}

	class opengl::program
		: public ooe::program
	{
	public:
		program( void );
		virtual ~program( void );

		virtual void bind( void );
		virtual void unbind( void );
		virtual void attach( const shader* );
		virtual void detach( const shader* );
		virtual void link( void );

		virtual ooe::uniform* uniform( const c8*, uniform::type ) const;
		virtual ooe::attribute* attribute( const c8*, attribute::type ) const;

	private:
		u32 id;
	};
}

#endif	// OOE_FOUNDATION_OPENGL_PROGRAM_HPP
