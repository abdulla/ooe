/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_NEXT_PROGRAM_HPP
#define OOE_FOUNDATION_OPENGL_NEXT_PROGRAM_HPP

#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

struct program
	: public ooe::program
{
	const u32 id;

	program( const shader_vector& );
	virtual ~program( void );

	virtual block_type block( const buffer_type& ) const;
};

OOE_NAMESPACE_END( ( ooe )( opengl ) )

#endif	// OOE_FOUNDATION_OPENGL_NEXT_PROGRAM_HPP
